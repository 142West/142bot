#include <142bot/modules.hpp>

#include <ldap.h>
#include <fmt/format.h>
#include <errno.h>

class LdapModule: public Module {
    private:
        LDAP* ldap_handle;
    public:
    LdapModule(Bot* creator, ModuleLoader* ml): Module(creator, ml) {
        // initialize libldap
        ldap_initialize(&this->ldap_handle, creator->cfg.value("ldap_uri", "ldap://localhost:389").c_str());
        creator->core->log(dpp::ll_debug, "Testing LDAP connection...");
        int err = 0;
        if ((err = ldap_connect(this->ldap_handle)) != LDAP_SUCCESS) {
            creator->core->log(dpp::ll_error, fmt::format("LDAP error: {}", ldap_err2string(errno)));
            return;
        }
        // bind to LDAP server
        auto bind_dn = creator->cfg.value("ldap_bind_dn", "cn=default,dc=example,dc=com");
        auto bind_pw = creator->cfg.value("ldap_bind_pw", "bad-password");
        if ((err = ldap_simple_bind(this->ldap_handle, bind_dn.c_str(), bind_pw.c_str())) != LDAP_SUCCESS) {
            creator->core->log(dpp::ll_error, fmt::format("LDAP error: {}", ldap_err2string(errno)));
            ldap_unbind(this->ldap_handle);
            return;
        } 
        ml->attach({I_OnSlashCommand}, this);
        auto id = creator->cfg.value("application_id", "");
        dpp::slashcommand ldap("ldap", "Control the 142 LDAP integration", id);

        ldap.add_option(
            /* sync subcommand */
            dpp::command_option(dpp::co_sub_command, "sync", "triggers an LDAP sync")
        );
        ldap.add_option(
            /* List roles subcommand */
            dpp::command_option(dpp::co_sub_command, "list", "List all roles in LDAP")
        );
        ldap.add_option(
            /* Me subcommand (prints LDAP data)*/
            dpp::command_option(dpp::co_sub_command, "me", "Gets LDAP information on you or another person")
                .add_option(dpp::command_option(dpp::co_mentionable, "member", "The other member to get data for"))
        );

        // register command
        creator->core->guild_command_create(ldap, creator->cfg["main_guild"]);
    }

    virtual std::string version() {
        return "0.1.0";
    }

    virtual std::string description() {
        return "LDAP integration";
    }

    bool OnSlashCommand(const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "ldap") {
            event.reply(dpp::message("Guess who got called!"));
        }

        return true;
    }

};

ENTRYPOINT(LdapModule)