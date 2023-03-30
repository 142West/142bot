
#include <stdlib.h>
#include <142bot/bot.hpp>
#include <dpp/dpp.h>
#include <142bot/modules.hpp>
#include <142bot/util.hpp>
#include <fmt/format.h>
#include <fmt/format-inl.h>

Bot::Bot(bool devel, dpp::cluster* cluster) {
    dev = devel;
    this->core = cluster;
    this->loader = new ModuleLoader(this);
    if (!this->loader->load("module_mmanager.so")) {
        this->core->log(dpp::ll_error, "Couldn't load modulemanager");
        exit(-1);
    }
//    this->loader->LoadAll();
}

bool Bot::isDevMode() {
    return dev;
}

int64_t Bot::getID() {
    return this->user.id;
}

/**
 * Announces that the bot is online. Each shard receives one of the events.
 */
void Bot::onReady(const dpp::ready_t& ready) {
	this->user = core->me;
	FOREACH_MOD(I_OnReady, OnReady(ready));
}

/**
 * Called on receipt of each message. We do our own cleanup of the message, sanitising any
 * mentions etc from the text before passing it along to modules. The bot's builtin ignore list
 * and a hard coded check against bots/webhooks and itself happen before any module calls,
 * and can't be overridden.
 */
void Bot::onMessage(const dpp::message_create_t &message) {

	if (!message.msg.author.id) {
		core->log(dpp::ll_info, fmt::format("Message dropped, no author: {}", message.msg.content));
		return;
	}
	/* Ignore self, and bots */
	if (message.msg.author.id != user.id && message.msg.author.is_bot() == false) {

		/* Replace all mentions with raw nicknames */
		bool mentioned = false;
		std::string mentions_removed = message.msg.content;
		std::vector<std::string> stringmentions;
		for (auto m = message.msg.mentions.begin(); m != message.msg.mentions.end(); ++m) {
			stringmentions.push_back(std::to_string(m->first.id));
			mentions_removed = ReplaceString(mentions_removed, std::string("<@") + std::to_string(m->first.id) + ">", m->first.username);
			mentions_removed = ReplaceString(mentions_removed, std::string("<@!") + std::to_string(m->first.id) + ">", m->first.username);
			if (m->first.id == user.id) {
				mentioned = true;
			}
		}

		std::string botusername = this->user.username;

		/* Remove bot's nickname from start of message, if it's there */
		while (mentions_removed.substr(0, botusername.length()) == botusername) {
			mentions_removed = trim(mentions_removed.substr(botusername.length(), mentions_removed.length()));
		}
		/* Remove linefeeds, they mess with botnix */
		mentions_removed = trim(mentions_removed);

		/* Call modules */
		FOREACH_MOD(I_OnMessage,OnMessage(message, mentions_removed, mentioned, stringmentions));
	}
}

void Bot::onChannel(const dpp::channel_create_t& channel_create) {
	FOREACH_MOD(I_OnChannelCreate, OnChannelCreate(channel_create));
}

void Bot::onChannelDelete(const dpp::channel_delete_t& cd) {
	FOREACH_MOD(I_OnChannelDelete, OnChannelDelete(cd));
}

void Bot::onServerDelete(const dpp::guild_delete_t& gd) {
	FOREACH_MOD(I_OnGuildDelete, OnGuildDelete(gd));
}

void Bot::onMember(const dpp::guild_member_add_t& gma) {
    FOREACH_MOD(I_OnGuildMemberAdd, OnGuildMemberAdd(gma));
}

void Bot::onPresenceUpdate(const dpp::presence_update_t &pu) {
    FOREACH_MOD(I_OnPresenceUpdate, OnPresenceUpdate());
}


void Bot::onTypingStart(const dpp::typing_start_t &obj)
{
	
}


void Bot::onMessageUpdate(const dpp::message_update_t &obj)
{
	
}


void Bot::onMessageDelete(const dpp::message_delete_t &obj)
{
	
}


void Bot::onMessageDeleteBulk(const dpp::message_delete_bulk_t &obj)
{
	
}


void Bot::onGuildUpdate(const dpp::guild_update_t &obj)
{
	
}


void Bot::onMessageReactionAdd(const dpp::message_reaction_add_t &obj)
{
	
}


void Bot::onMessageReactionRemove(const dpp::message_reaction_remove_t &obj)
{
	
}


void Bot::onMessageReactionRemoveAll(const dpp::message_reaction_remove_all_t &obj)
{
	
}


void Bot::onUserUpdate(const dpp::user_update_t &obj)
{
	
}


void Bot::onResumed(const dpp::resumed_t &obj)
{
	
}


void Bot::onChannelUpdate(const dpp::channel_update_t &obj)
{
	
}


void Bot::onChannelPinsUpdate(const dpp::channel_pins_update_t &obj)
{
	
}


void Bot::onGuildBanAdd(const dpp::guild_ban_add_t &obj)
{
	
}


void Bot::onGuildBanRemove(const dpp::guild_ban_remove_t &obj)
{
	
}


void Bot::onGuildEmojisUpdate(const dpp::guild_emojis_update_t &obj)
{
	
}


void Bot::onGuildIntegrationsUpdate(const dpp::guild_integrations_update_t &obj)
{
	
}


void Bot::onGuildMemberRemove(const dpp::guild_member_remove_t &obj)
{
	
}


void Bot::onGuildMemberUpdate(const dpp::guild_member_update_t &obj)
{
	
}


void Bot::onGuildMembersChunk(const dpp::guild_members_chunk_t &obj)
{
	
}


void Bot::onGuildRoleCreate(const dpp::guild_role_create_t &obj)
{
	
}


void Bot::onGuildRoleUpdate(const dpp::guild_role_update_t &obj)
{
	
}


void Bot::onGuildRoleDelete(const dpp::guild_role_delete_t &obj)
{
	
}




void Bot::onVoiceStateUpdate(const dpp::voice_state_update_t &obj)
{
	
}


void Bot::onVoiceServerUpdate(const dpp::voice_server_update_t &obj)
{
	
}


void Bot::onWebhooksUpdate(const dpp::webhooks_update_t &obj)
{
	
}

