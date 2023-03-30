/*
 * =====================================================================================
 *
 *       Filename:  mmanager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/30/2023 11:30:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Cara Salter (muirrum), cara@devcara.com
 *   Organization:  Worcester Polytechnic Institute
 *
 * =====================================================================================
 */
#include <dpp/message.h>
#include <stdlib.h>
#include <string>
#include <142bot/modules.hpp>
#include <dpp/dpp.h>

class MManagerModule : public Module {
    double microseconds_ping;
public:
    MManagerModule(Bot* creator, ModuleLoader* ml) : Module(creator, ml) {
        ml->attach({ I_OnMessage, I_OnReady }, this);
        creator->core->log(dpp::ll_info, "ModuleManager online!");
    }

    virtual std::string version() {
        return "0.1.0";
    }

    virtual std::string description() {
        return "module manager";
    }

    virtual bool OnReady(const dpp::ready_t &ready) {
        bot->core->log(dpp::ll_info, "Got ready event");
        return true;
    }

	virtual bool OnMessage(const dpp::message_create_t &message, const std::string& clean_message, bool mentioned, const std::vector<std::string> &stringmentions) {
        bot->core->log(dpp::ll_info, "Got message!");

        return true;
    }
};

ENTRYPOINT(MManagerModule)
