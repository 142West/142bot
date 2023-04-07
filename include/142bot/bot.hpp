#include <dpp/snowflake.h>
#include <dpp/user.h>
#include <dpp/dpp.h>
#include <pqxx/pqxx>


#ifndef BOT_HPP
#define BOT_HPP
class Module;
class ModuleLoader;
class Bot {
    bool dev;
    dpp::snowflake owner_id;
private:
	bool run_database_migrations();
public:
    char prefix;
	pqxx::connection conn;
    class dpp::cluster * core;
    /*  The bot's user from the ready event */
    dpp::user user;

    Bot(bool development, dpp::cluster* cluster, char prefix);
    //virtual ~Bot();

    void set_owner_id(dpp::snowflake id);
    dpp::snowflake get_owner_id();

	ModuleLoader* loader;

    bool isDevMode();

    int64_t getID();

  	void onReady(const dpp::ready_t &ready);
	void onServer(const dpp::guild_create_t &gc);
	void onMember(const dpp::guild_member_add_t &gma);
	void onChannel(const dpp::channel_create_t &channel);
	void onMessage(const dpp::message_create_t &message);
	void onChannelDelete(const dpp::channel_delete_t &cd);
	void onServerDelete(const dpp::guild_delete_t &gd);
	void onTypingStart (const dpp::typing_start_t &event);
	void onMessageUpdate (const dpp::message_update_t &event);
	void onMessageDelete (const dpp::message_delete_t &event);
	void onMessageDeleteBulk (const dpp::message_delete_bulk_t &event);
	void onGuildUpdate (const dpp::guild_update_t &event);
	void onMessageReactionAdd (const dpp::message_reaction_add_t &event);
	void onMessageReactionRemove (const dpp::message_reaction_remove_t &event);
	void onMessageReactionRemoveAll (const dpp::message_reaction_remove_all_t &event);
	void onUserUpdate (const dpp::user_update_t &event);
	void onResumed (const dpp::resumed_t &event);
	void onChannelUpdate (const dpp::channel_update_t &event);
	void onChannelPinsUpdate (const dpp::channel_pins_update_t &event);
	void onGuildBanAdd (const dpp::guild_ban_add_t &event);
	void onGuildBanRemove (const dpp::guild_ban_remove_t &event);
	void onGuildEmojisUpdate (const dpp::guild_emojis_update_t &event);
	void onGuildIntegrationsUpdate (const dpp::guild_integrations_update_t &event);
	void onGuildMemberRemove (const dpp::guild_member_remove_t &event);
	void onGuildMemberUpdate (const dpp::guild_member_update_t &event);
	void onGuildMembersChunk (const dpp::guild_members_chunk_t &event);
	void onGuildRoleCreate (const dpp::guild_role_create_t &event);
	void onGuildRoleUpdate (const dpp::guild_role_update_t &event);
	void onGuildRoleDelete (const dpp::guild_role_delete_t &event);
	void onPresenceUpdate (const dpp::presence_update_t &event);
	void onVoiceStateUpdate (const dpp::voice_state_update_t &event);
	void onVoiceServerUpdate (const dpp::voice_server_update_t &event);
	void onWebhooksUpdate (const dpp::webhooks_update_t &event);
};

#endif
