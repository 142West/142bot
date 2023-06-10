#include <142bot/modules.hpp>
#include <142bot/util.hpp>
#include <dpp/dpp.h>

class MailModule: public Module {
    public:
    MailModule(Bot* creator, ModuleLoader* ml) : Module(creator, ml) {
        ml->attach({I_OnSlashCommand, I_OnFormSubmit}, this);
        auto id = creator->cfg.value("application_id", "");
        creator->core->log(dpp::ll_info, "Application ID: " + id);
        creator->core->guild_command_create(dpp::slashcommand("mailalert", "Alert a resident that they have mail", id), creator->cfg["main_guild"]);
        creator->core->log(dpp::ll_info, "Registered mailalert");
    }

    virtual std::string version() {
        return "0.1.0";
    }

    virtual std::string description() {
        return "Alerts residents when mail has arrived";
    }

    bool OnSlashCommand(const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "mailalert") {
            dpp::interaction_modal_response dialog("mail", "Mail alert!");

            dialog.add_component(
                    dpp::component()
                    .set_label("Sender")
                    .set_type(dpp::component_type::cot_text)
                    .set_id("sender")
                    .set_placeholder("Boom boom")
                    .set_text_style(dpp::text_style_type::text_short)
                    );

            auto select_comp = dialog.add_component(
                    dpp::component()
                    .set_label("Recipient")
                    .set_id("recipient")
                    .set_type(dpp::component_type::cot_mentionable_selectmenu)
                    );
        }
        return true;
    }

    bool OnFormSubmit(const dpp::form_submit_t &event) {

        return true;
    }
};

ENTRYPOINT(MailModule)
