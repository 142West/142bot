#include <142bot/modules.hpp>
#include <142bot/util.hpp>
#include <dpp/dpp.h>
#include <cpr/cpr.h>
#include <fmt/format.h>

class StoplightModule: public Module {
    std::string stoplightBaseUrl;
    public:
    StoplightModule(Bot* creator, ModuleLoader* ml): Module(creator,ml) {
        ml->attach({I_OnSlashCommand}, this);
        auto id = creator->cfg.value("application_id", "");
        dpp::slashcommand stoplight("stoplight", "Control the 142 stop light", creator->core->me.id);
        stoplight.add_option(
            /* Stop subcommand*/
            dpp::command_option(dpp::co_sub_command, "stop", "Stops the stoplight")
        );
        stoplight.add_option(
            /* start subcommand */
            dpp::command_option(dpp::co_sub_command, "start", "Starts the stoplight")
        );
        stoplight.add_option(
            /* shots */
            dpp::command_option(dpp::co_sub_command, "shots", "Triggers shots alarm subroutine")
                .add_option(dpp::command_option(dpp::co_integer, "countdowntime", "Time to count down for", true))
                .add_option(dpp::command_option(dpp::co_integer, "gotime", "Time to shots for", true))
        );

        // register command
        creator->core->guild_command_create(stoplight, creator->cfg["main_guild"]);
        creator->core->log(dpp::ll_info, "Registered stoplight");

        this->stoplightBaseUrl = creator->cfg["stoplight_base"];

    }

    virtual std::string version() {
        return "0.1.0";
    }

    virtual std::string description() {
        return "Stoplight control";
    }

    bool OnSlashCommand(const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "stoplight") {
            dpp::command_interaction cmd_data = event.command.get_command_interaction();

            auto subcommand = cmd_data.options[0];

            this->bot->core->log(dpp::ll_debug, fmt::format("Got subcommand: {}", subcommand.name));

            if (subcommand.name == "start") {
               bot->core->log(dpp::ll_debug, "Attempting to start stoplight");
                cpr::Response r = cpr::Get(cpr::Url(fmt::format("{}/start", this->stoplightBaseUrl)));

                if (r.status_code != 200) {
                    bot->core->log(dpp::ll_error, r.text);
                    throw std::exception();
                }
                event.reply(dpp::message("Started stoplight"));
                
            } else if (subcommand.name == "stop") {
                bot->core->log(dpp::ll_debug, "Attempting to stop stoplight");
                cpr::Response r = cpr::Get(cpr::Url(fmt::format("{}/stop", this->stoplightBaseUrl)));

                if (r.status_code != 200) {
                    bot->core->log(dpp::ll_error, r.text);
                    throw std::exception();
                }
                event.reply(dpp::message("Stopped stoplight"));
                
            }
        }
        return true;
    }
};


ENTRYPOINT(StoplightModule)