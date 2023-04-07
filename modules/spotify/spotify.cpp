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
#include <pcre.h>
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
        sentry_set_tag("module", "spotify");
        bot->core->log(dpp::ll_debug, "Got message event");        

        const char* pcre_error;
        int pcre_error_ofs;

        auto comp = pcre_compile("^https:\/\/open.spotify.com\/track\/([a-zA-Z0-9]+)(.*)$", PCRE_CASELESS, &pcre_error, &pcre_error_ofs, NULL);
        if (!comp) {
            bot->core->log(dpp::ll_error, pcre_error);
        }

        int matcharr[90];
        int matchcount = pcre_exec(comp, NULL, clean_message.c_str(), clean_message.length(), 0, 0, matcharr, 90);

        if (matchcount > 0) {
            // We found a spotify URL!
            EmbedSimple("Found a spotify URL", message.msg.channel_id);
        } else {
            if (clean_message.starts_with(bot->prefix)) {

            }
        }

        return true;
    }
};



ENTRYPOINT(SpotifyModule)
