#include <142bot/modules.hpp>
#include <142bot/util.hpp>
#include <dpp/dpp.h>
#include <fmt/format.h>

class MailModule: public Module {
    public:
    MailModule(Bot* creator, ModuleLoader* ml) : Module(creator, ml) {
        ml->attach({I_OnSlashCommand}, this);
        auto id = creator->cfg.value("application_id", "");
        creator->core->log(dpp::ll_info, "Application ID: " + id);
        creator->core->guild_command_create(dpp::slashcommand("mailalert", "Alert a resident that they have mail", id)
                                            .add_option(dpp::command_option(dpp::co_string, "sender", "Who sent the mail?", true))
                                            .add_option(dpp::command_option(dpp::co_user, "recipient", "Who's the mail for?", true)), creator->cfg["main_guild"]);
        creator->core->log(dpp::ll_info, "Registered mailalert");
    }

    virtual std::string version() {
        return "0.1.1";
    }

    virtual std::string description() {
        return "Alerts residents when mail has arrived";
    }

    bool OnSlashCommand(const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "mailalert") {
            dpp::snowflake recipient = std::get<dpp::snowflake>(event.get_parameter("recipient"));
            std::string sender = std::get<std::string>(event.get_parameter("sender"));

            auto rec = dpp::find_guild_member(bot->cfg.value("main_guild", ""), recipient);
            auto roles = rec.roles;
            for (int i = 0; i < roles.size(); ++i) {
                if (roles[i] == 1083010579406536705u) {
                    std::string msg = fmt::format("**ALERT. ALERT.**\nMAIL STORAGE LEVELS CRITICAL. NEW MAIL FOR...\n\n{}...\n\nFROM...\n\n{}", rec.get_mention(), sender);
                    event.reply(dpp::message(msg).set_allowed_mentions(true, false, false, false, {}, {}));
                    return true;
                }
            }

            event.reply(dpp::message("**USER NOT RESIDENT. PLEASE TRY AGAIN.**").set_flags(dpp::m_ephemeral));
        }
        return true;
    }


};

ENTRYPOINT(MailModule)
