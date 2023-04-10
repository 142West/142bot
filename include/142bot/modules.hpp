
/**
 * Thinking about modules --
 * 
 * Essentially what we're doing is loading a .so file at runtime. That then gets turned into everyone's favorite, modules!
*/

#pragma once

#include "bot.hpp"
#include <sentry.h>
class ModuleLoader;
class Module;

/**
 * Enum of possible events to be attached to
*/
enum Events
{
	I_BEGIN,
	I_OnMessage,
	I_OnReady,
	I_OnChannelCreate,
	I_OnChannelDelete,
	I_OnGuildMemberAdd,
	I_OnGuildCreate,
	I_OnGuildDelete,
	I_OnPresenceUpdate,
	I_OnRestEnd,
	I_OnAllShardsReady,
	I_OnTypingStart,
	I_OnMessageUpdate,
	I_OnMessageDelete,
	I_OnMessageDeleteBulk,
	I_OnGuildUpdate,
	I_OnMessageReactionAdd,
	I_OnMessageReactionRemove,
	I_OnMessageReactionRemoveAll,
	I_OnUserUpdate,
	I_OnResumed,
	I_OnChannelUpdate,
	I_OnChannelPinsUpdate,
	I_OnGuildBanAdd,
	I_OnGuildBanRemove,
	I_OnGuildEmojisUpdate,
	I_OnGuildIntegrationsUpdate,
	I_OnGuildMemberRemove,
	I_OnGuildMemberUpdate,
	I_OnGuildMembersChunk,
	I_OnGuildRoleCreate,
	I_OnGuildRoleUpdate,
	I_OnGuildRoleDelete,
	I_OnPresenceUpdateWS,
	I_OnVoiceStateUpdate,
	I_OnVoiceServerUpdate,
	I_OnWebhooksUpdate,
	I_OnCommand,
	I_END
};

/**
 * This #define allows us to call a method in all loaded modules in a readable simple way, e.g.:
 * 'FOREACH_MOD(I_OnGuildAdd,OnGuildAdd(guildinfo));'
 * NOTE: Locks mutex - two FOREACH_MOD() can't run asyncronously in two different threads. This is
 * to prevent one thread loading/unloading a module, changing the arrays/vectors while another thread
 * is running an event.
 */
#define FOREACH_MOD(y,x) { \
	std::vector<Module*> list_to_call; \
	{ \
		std::lock_guard l(loader->mtx); \
		list_to_call = loader->EventHandlers[y]; \
	} \
	for (auto _i = list_to_call.begin(); _i != list_to_call.end(); ++_i) \
	{ \
		core->log(dpp::ll_debug, fmt::format("Attempting to call module")); \
		try \
		{ \
			if (!(*_i)->x) {        \
                list_to_call = loader->EventHandlers[y];           \
				core->log(dpp::ll_error, "Something happened!"); \
				break; \
			} \
			sentry_remove_tag("module"); \
			core->log(dpp::ll_debug, "called module 1"); \
		} \
		catch (std::exception& modexcept) \
		{ \
			core->log(dpp::ll_error, fmt::format("Exception caught in module: {}", modexcept.what())); \
			sentry_value_t event = sentry_value_new_event(); \
			sentry_value_t exc = sentry_value_new_exception("Exception", modexcept.what()); \
			sentry_value_set_stacktrace(exc, NULL, 0); \
			sentry_event_add_exception(event, exc); \
			sentry_capture_event(event); \
		} \
		core->log(dpp::ll_debug, "Called module"); \
	} \
};

// Module entry function type
typedef Module* (initfunctype) (Bot*, ModuleLoader*);

// Represents a mapping of module names to modules
typedef std::map<std::string, Module*> ModuleMap;

// Contains the low-level stuff that goes into a module:
struct ModuleLowLevel {
    initfunctype* init;
    void* dlopen_handle;
    Module* mod;
};

/**
 * The dynamic loader class. Contains details on loaded modules and knows how to load and unload them
*/
class ModuleLoader {
    Bot* bot;

    // Loaded module list
    std::map<std::string, ModuleLowLevel> Modules;

    // Gets a named symbol from the module
    bool get_symbol(ModuleLowLevel &lowlevel, const char* name);

    // Externally-provided module map
    ModuleMap mod_map;

public:
    // Module mutex
    std::mutex mtx;

    // List of registered event handlers
    std::vector<Module*> EventHandlers[I_END];

    ModuleLoader(Bot* creator);

    // Attach a module to an event
    void attach(const std::vector<Events> &e, Module* m);

    // Detach a module from an event
    void detach(const std::vector<Events> &e, Module* m);

    // Load a module
    bool load(const std::string &fname);

    // Unload a module
    bool unload(const std::string &fname);

    // Unloads and then reloads a module
    bool reload(const std::string &fname);

	void load_all();

    // Get list of loaded modules
    const ModuleMap& get_loaded_modules() const;
};

// Defines an individual module
class Module {
protected:
    Bot* bot;
public:
    Module(Bot* caller, ModuleLoader* ml);
    virtual ~Module();

    virtual std::string version();
    virtual std::string description();

    virtual bool OnChannelCreate(const dpp::channel_create_t &channel);
	virtual bool OnReady(const dpp::ready_t &ready);
	virtual bool OnChannelDelete(const dpp::channel_delete_t &channel);
	virtual bool OnGuildCreate(const dpp::guild_create_t &guild);
	virtual bool OnGuildDelete(const dpp::guild_delete_t &guild);
	virtual bool OnGuildMemberAdd(const dpp::guild_member_add_t &gma);
	virtual bool OnMessage(const dpp::message_create_t &message, const std::string& clean_message, bool mentioned, const std::vector<std::string> &stringmentions);
	virtual bool OnPresenceUpdate();
	virtual bool OnAllShardsReady();
	virtual bool OnTypingStart(const dpp::typing_start_t &obj);
	virtual bool OnMessageUpdate(const dpp::message_update_t &obj);
	virtual bool OnMessageDelete(const dpp::message_delete_t &obj);
	virtual bool OnMessageDeleteBulk(const dpp::message_delete_bulk_t &obj);
	virtual bool OnGuildUpdate(const dpp::guild_update_t &obj);
	virtual bool OnMessageReactionAdd(const dpp::message_reaction_add_t &obj);
	virtual bool OnMessageReactionRemove(const dpp::message_reaction_remove_t &obj);
	virtual bool OnMessageReactionRemoveAll(const dpp::message_reaction_remove_all_t &obj);
	virtual bool OnUserUpdate(const dpp::user_update_t &obj);
	virtual bool OnResumed(const dpp::resumed_t &obj);
	virtual bool OnChannelUpdate(const dpp::channel_update_t &obj);
	virtual bool OnChannelPinsUpdate(const dpp::channel_pins_update_t &obj);
	virtual bool OnGuildBanAdd(const dpp::guild_ban_add_t &obj);
	virtual bool OnGuildBanRemove(const dpp::guild_ban_remove_t &obj);
	virtual bool OnGuildEmojisUpdate(const dpp::guild_emojis_update_t &obj);
	virtual bool OnGuildIntegrationsUpdate(const dpp::guild_integrations_update_t &obj);
	virtual bool OnGuildMemberRemove(const dpp::guild_member_remove_t &obj);
	virtual bool OnGuildMemberUpdate(const dpp::guild_member_update_t &obj);
	virtual bool OnGuildMembersChunk(const dpp::guild_members_chunk_t &obj);
	virtual bool OnGuildRoleCreate(const dpp::guild_role_create_t &obj);
	virtual bool OnGuildRoleUpdate(const dpp::guild_role_update_t &obj);
	virtual bool OnGuildRoleDelete(const dpp::guild_role_delete_t &obj);
	virtual bool OnPresenceUpdateWS(const dpp::presence_update_t &obj);
	virtual bool OnVoiceStateUpdate(const dpp::voice_state_update_t &obj);
	virtual bool OnVoiceServerUpdate(const dpp::voice_server_update_t &obj);
	virtual bool OnWebhooksUpdate(const dpp::webhooks_update_t &obj);
	virtual bool OnCommand(const dpp::message_create_t &message, const std::string &command, const std::vector<std::string>& params);
	void EmbedSimple(const std::string &message, int64_t channelID);
};

/* A macro that lets us simply define the entrypoint of a module by name */
#define ENTRYPOINT(mod_class_name) extern "C" Module* init_module(Bot* instigator, ModuleLoader* ml) { return new mod_class_name(instigator, ml); }
