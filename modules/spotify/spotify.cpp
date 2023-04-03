/*
 * =====================================================================================
 *
 *       Filename:  spotify.cpp
 *
 *    Description:  Implementation of the spotify API 
 *
 *        Version:  1.0
 *        Created:  04/01/2023 09:55:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Cara Salter (muirrum), cara@devcara.com
 *   Organization:  Worcester Polytechnic Institute
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "cpr/cpr.h"
#include <142bot/modules.hpp>


class SpotifyModule: public Module {
    std::string spotifyRegex;
public:
    SpotifyModule(Bot* creator, ModuleLoader* ml) : Module(creator, ml) {
        ml->attach({I_OnMessage}, this);

        this->spotifyRegex = "^https:\/\/open.spotify.com\/track\/([a-zA-Z0-9]+)(.*)$";
    }

    virtual std::string version() {
        return "0.1.0";
    }

    virtual std::string description() {
        return "Manage spotify queues for 142";
    }

    virtual bool OnMessage(const dpp::message_create_t &message, const std::string& clean_message, bool mentioned, const std::vector<std::string> & mentions) {
        

        return true;
    }
};



ENTRYPOINT(SpotifyModule)
