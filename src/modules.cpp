#include <dlfcn.h>
#include <dpp/dpp.h>

#include <142bot/modules.hpp>
#include <fmt/format.h>
#include <mutex>
#include <limits.h>
#include <link.h>

const char* StringNames[I_END + 1] = {
	"I_BEGIN",
	"I_OnMessage",
	"I_OnReady",
	"I_OnChannelCreate",
	"I_OnChannelDelete",
	"I_OnGuildMemberAdd",
	"I_OnGuildCreate",
	"I_OnGuildDelete",
	"I_OnPresenceUpdate",
	"I_OnRestEnd",
	"I_OnAllShardsReady",
	"I_OnTypingStart",
	"I_OnMessageUpdate",
	"I_OnMessageDelete",
	"I_OnMessageDeleteBulk",
	"I_OnGuildUpdate",
	"I_OnMessageReactionAdd",
	"I_OnMessageReactionRemove",
	"I_OnMessageReactionRemoveAll",
	"I_OnUserUpdate",
	"I_OnResumed",
	"I_OnChannelUpdate",
	"I_OnChannelPinsUpdate",
	"I_OnGuildBanAdd",
	"I_OnGuildBanRemove",
	"I_OnGuildEmojisUpdate",
	"I_OnGuildIntegrationsUpdate",
	"I_OnGuildMemberRemove",
	"I_OnGuildMemberUpdate",
	"I_OnGuildMembersChunk",
	"I_OnGuildRoleCreate",
	"I_OnGuildRoleUpdate",
	"I_OnGuildRoleDelete",
	"I_OnPresenceUpdateWS",
	"I_OnVoiceStateUpdate",
	"I_OnVoiceServerUpdate",
	"I_OnWebhooksUpdate",
	"I_END"
};


ModuleLoader::ModuleLoader(Bot* creator): bot(creator) {
    bot->core->log(dpp::ll_info, "Module loader init");
}

// Attach a module to an event dynamically
void ModuleLoader::attach(const std::vector<Events> &i, Module* m) {
    for (auto n = i.begin(); n != i.end(); ++n) {
        if (std::find(EventHandlers[*n].begin(), EventHandlers[*n].end(), m) == EventHandlers[*n].end()) {
            EventHandlers[*n].push_back(m);
            bot->core->log(dpp::ll_info, fmt::format("Module {} attached to event {}", m->description(), StringNames[*n]));
        } else {
            bot->core->log(dpp::ll_warning, fmt::format("Module {} already attached to event {}", m->description(), StringNames[*n]));
        }
    }
}

// Detach a module from an event
void ModuleLoader::detach(const std::vector<Events> &e, Module *m) {
    for (auto n = e.begin(); n != e.end(); ++n) {
        auto it = std::find(EventHandlers[*n].begin(), EventHandlers[*n].end(), m);
        if (it != EventHandlers[*n].end()) {
            EventHandlers[*n].erase(it);
            bot->core->log(dpp::ll_info, fmt::format("Module {} detached from event {}", m->description(), StringNames[*n]));
        }
    }
}

// Get Loaded module list
const ModuleMap& ModuleLoader::get_loaded_modules() const {
    return this->mod_map;
}

// Load a new module
bool ModuleLoader::load(const std::string &fname) {

    ModuleLowLevel m;
    m.dlopen_handle = nullptr;
    m.init = nullptr;
    m.mod = nullptr;

    bot->core->log(dpp::ll_info, fmt::format("Attempting to load module {}", fname));

    std::lock_guard l(this->mtx);

    if (Modules.find(fname) == Modules.end()) {
        char buffer[PATH_MAX + 1];
        getcwd(buffer, PATH_MAX);
        std::string full_path = std::string(buffer) + "/" + fname;

        m.dlopen_handle = dlopen(full_path.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!m.dlopen_handle) {
            bot->core->log(dpp::ll_error, fmt::format("Can't load module: {}", dlerror()));
            return false;
        } else {
            if (!get_symbol(m, "init_module")) {
                bot->core->log(dpp::ll_error, fmt::format("Could not find init_module symbol"));
                dlclose(m.dlopen_handle);
                return false;
            } else {
                bot->core->log(dpp::ll_debug, "Symbol found, attempting to load module");
                m.mod = m.init(bot, this);

                if (!m.mod || (uint64_t)m.mod == 0xffffffffffffffff) {
                    bot->core->log(dpp::ll_error, "Invalid module pointer returned");
                    dlclose(m.dlopen_handle);
                    return false;
                } else {
                    bot->core->log(dpp::ll_info, fmt::format("Loaded {} ({})", m.mod->description(), m.mod->version()));
                    Modules[fname] = m;
                    mod_map[fname] = m.mod;
                    return true;
                }
            }
        }
    } else {
        bot->core->log(dpp::ll_error, "Module already loaded");
        return false;
    }
    return true;
}

// Unloads a module
bool ModuleLoader::unload(const std::string &fname) {
    std::lock_guard l(mtx);

    auto m = Modules.find(fname);

    if (m == Modules.end()) {
        bot->core->log(dpp::ll_error, "Module not loaded");
        return false;
    }

    ModuleLowLevel& mod = m->second;

    // Remove attached events
    for (int j = I_BEGIN; j != I_END; ++j) {
        		auto p = std::find(EventHandlers[j].begin(), EventHandlers[j].end(), mod.mod);
		if (p != EventHandlers[j].end()) {
			EventHandlers[j].erase(p);
			bot->core->log(dpp::ll_debug, fmt::format("Removed event {} from {}", StringNames[j], fname));
		}	
    }

    Modules.erase(m);

    auto v = mod_map.find(fname);
    if (v != mod_map.end()) {
        mod_map.erase(v);
        bot->core->log(dpp::ll_debug, "Removed module from public list");
    }

    if (mod.mod) {
        delete mod.mod;
    }

    if (mod.dlopen_handle) {
        dlclose(mod.dlopen_handle);
        bot->core->log(dpp::ll_debug, "Unloaded module");
    }

    return true;
    
}


bool ModuleLoader::reload(const std::string &filename)
{
	/* Short-circuit evaluation here means that if Unload() returns false,
	 * Load() won't be called at all.
	 */
	return (unload(filename) && load(filename));
}

/**
 * Return a given symbol name from a shared object represented by the ModuleNative value.
 */
bool ModuleLoader::get_symbol(ModuleLowLevel &native, const char *sym_name)
{
	/* Find exported symbol in shared object */
	if (native.dlopen_handle) {
		dlerror(); // clear value
		native.init = (initfunctype*)dlsym(native.dlopen_handle, sym_name);
		//printf("dlopen_handle=0x%016x, native.init=0x%016x native.err=\"%s\" dlsym=0x%016x sym_name=%s\n", native.dlopen_handle, native.init, native.err ? native.err : "<NULL>", dlsym(native.dlopen_handle, sym_name), sym_name);

	} else {
		bot->core->log(dpp::ll_error, "ModuleLoader::GetSymbol(): Invalid dlopen() handle");
		return false;
	}
	return true;
}

Module::Module(Bot* instigator, ModuleLoader* ml) : bot(instigator)
{
}

Module::~Module()
{
}

std::string Module::version()
{
	return "";
}

std::string Module::description()
{
	return "";
}

bool Module::OnChannelCreate(const dpp::channel_create_t &channel)
{
	return true;
} 

bool Module::OnReady(const dpp::ready_t &ready)
{
	return true;
}

bool Module::OnChannelDelete(const dpp::channel_delete_t &channel)
{
	return true;
}

bool Module::OnGuildCreate(const dpp::guild_create_t &guild)
{
	return true;
}

bool Module::OnGuildDelete(const dpp::guild_delete_t &guild)
{
	return true;
}

bool Module::OnGuildMemberAdd(const dpp::guild_member_add_t &gma)
{
	return true;
}

bool Module::OnMessage(const dpp::message_create_t &message, const std::string& clean_message, bool mentioned, const std::vector<std::string> &stringmentions)
{
	return true;
}

bool Module::OnPresenceUpdate()
{
	return true;
}

bool Module::OnTypingStart(const dpp::typing_start_t &obj)
{
	return true;
}


bool Module::OnMessageUpdate(const dpp::message_update_t &obj)
{
	return true;
}


bool Module::OnMessageDelete(const dpp::message_delete_t &obj)
{
	return true;
}


bool Module::OnMessageDeleteBulk(const dpp::message_delete_bulk_t &obj)
{
	return true;
}


bool Module::OnGuildUpdate(const dpp::guild_update_t &obj)
{
	return true;
}


bool Module::OnMessageReactionAdd(const dpp::message_reaction_add_t &obj)
{
	return true;
}


bool Module::OnMessageReactionRemove(const dpp::message_reaction_remove_t &obj)
{
	return true;
}


bool Module::OnMessageReactionRemoveAll(const dpp::message_reaction_remove_all_t &obj)
{
	return true;
}


bool Module::OnUserUpdate(const dpp::user_update_t &obj)
{
	return true;
}


bool Module::OnResumed(const dpp::resumed_t &obj)
{
	return true;
}


bool Module::OnChannelUpdate(const dpp::channel_update_t &obj)
{
	return true;
}


bool Module::OnChannelPinsUpdate(const dpp::channel_pins_update_t &obj)
{
	return true;
}


bool Module::OnGuildBanAdd(const dpp::guild_ban_add_t &obj)
{
	return true;
}


bool Module::OnGuildBanRemove(const dpp::guild_ban_remove_t &obj)
{
	return true;
}


bool Module::OnGuildEmojisUpdate(const dpp::guild_emojis_update_t &obj)
{
	return true;
}


bool Module::OnGuildIntegrationsUpdate(const dpp::guild_integrations_update_t &obj)
{
	return true;
}


bool Module::OnGuildMemberRemove(const dpp::guild_member_remove_t &obj)
{
	return true;
}


bool Module::OnGuildMemberUpdate(const dpp::guild_member_update_t &obj)
{
	return true;
}


bool Module::OnGuildMembersChunk(const dpp::guild_members_chunk_t &obj)
{
	return true;
}


bool Module::OnGuildRoleCreate(const dpp::guild_role_create_t &obj)
{
	return true;
}


bool Module::OnGuildRoleUpdate(const dpp::guild_role_update_t &obj)
{
	return true;
}


bool Module::OnGuildRoleDelete(const dpp::guild_role_delete_t &obj)
{
	return true;
}


bool Module::OnPresenceUpdateWS(const dpp::presence_update_t &obj)
{
	return true;
}


bool Module::OnVoiceStateUpdate(const dpp::voice_state_update_t &obj)
{
	return true;
}


bool Module::OnVoiceServerUpdate(const dpp::voice_server_update_t &obj)
{
	return true;
}


bool Module::OnWebhooksUpdate(const dpp::webhooks_update_t &obj)
{
	return true;
}

bool Module::OnAllShardsReady()
{
	return true;
}



/**
 * Output a simple embed to a channel consisting just of a message.
 */
void Module::EmbedSimple(const std::string &message, int64_t channelID)
{
	std::stringstream s;
	json embed_json;

	s << "{\"color\":16767488, \"description\": \"" << message << "\"}";

	try {
		embed_json = json::parse(s.str());
	}
	catch (const std::exception &e) {
		bot->core->log(dpp::ll_error, fmt::format("Invalid json for channel {} created by EmbedSimple: ", channelID, s.str()));
	}
	dpp::channel* channel = dpp::find_channel(channelID);
	if (channel) {
			dpp::message m;
			m.channel_id = channel->id;
			m.embeds.push_back(dpp::embed(&embed_json));
			bot->core->message_create(m);
		
	} else {
		bot->core->log(dpp::ll_error, fmt::format("Invalid channel {} passed to EmbedSimple", channelID));
	}
}