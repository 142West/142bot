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
#include <regex>
#include <stdlib.h>
#include "cpr/cpr.h"
#include <142bot/modules.hpp>
#include <142bot/util.hpp>
#include <142bot/db.hpp>
#include <fmt/format.h>
#include <chrono>

using json = nlohmann::json;


class SpotifyModule: public Module {
    std::string spotifyRegex;
    std::string defaultSpotifyAccount;
    std::string spotifyBaseUrl;
    std::string spotifyDefaultDevice;
public:
    SpotifyModule(Bot* creator, ModuleLoader* ml) : Module(creator, ml) {
        ml->attach({I_OnMessage, I_OnCommand}, this);

        this->spotifyRegex = "^https:\/\/open.spotify.com\/track\/([a-zA-Z0-9]+)(.*)$";

        pqxx::work tx(creator->conn);
        try {
            auto rs = tx.exec_prepared1("state", "default_spotify_account");
            this->defaultSpotifyAccount = rs[0].as<std::string>();
        } catch (std::exception &e) {
            creator->core->log(dpp::ll_warning, "Couldn't find default_spotify_account in state, creating");
            tx.exec_prepared("update_state", "default_spotify_account", "1");
        } 
        this->spotifyBaseUrl = "https://api.spotify.com/v1";
        try {
            auto rs = tx.exec_prepared1("state", "default_spotify_device");
            this->spotifyDefaultDevice = rs[0].as<std::string>();
        } catch (std::exception &e) {
            creator->core->log(dpp::ll_warning, "Couldn't find default_spotify_device in state, creating");
            tx.exec_prepared("update_state", "default_spotify_device", "");
        }
        tx.commit();
    }

    virtual std::string version() {
        return "0.1.0";
    }

    virtual std::string description() {
        return "Manage spotify queues for 142";
    }

    void refreshSpotify(std::string refreshToken) {
        bot->core->log(dpp::ll_debug, "Attempting to refresh spotify token...");
        cpr::Response r = cpr::Post(cpr::Url("https://accounts.spotify.com/api/token"), 
            cpr::Authentication{bot->cfg["spotify"]["id"], bot->cfg["spotify"]["secret"], cpr::AuthMode::BASIC}, cpr::Payload{{"grant_type", "refresh_token"}, {"refresh_token", refreshToken}});

        bot->core->log(dpp::ll_trace, "Made request");

        if (r.status_code != 200) {
            bot->core->log(dpp::ll_error, r.text);
            throw std::exception();
        }
        bot->core->log(dpp::ll_trace, "Request successful");
        auto tmp = json::parse(r.text);
        bot->core->log(dpp::ll_trace, "Parsed JSON");

        uint64_t expires = tmp["expires_in"].get<uint64_t>();

        pqxx::work tx(bot->conn);
        asdf::timestamp parsed_expires = asdf::from_unix_time(time(0) + expires);
        bot->core->log(dpp::ll_trace, fmt::format("Got expires_in: {}", asdf::to_iso8601_str(parsed_expires)));
        std::string access = tmp["access_token"].get<std::string>();
        bot->core->log(dpp::ll_trace, fmt::format("Got access token"));
        tx.exec_params("UPDATE spotify SET spotify_token=$1, spotify_token_expires=$2 WHERE id=$3", access, parsed_expires, this->defaultSpotifyAccount);
        bot->core->log(dpp::ll_trace, "Updated DB");
        tx.commit();
        bot->core->log(dpp::ll_debug, "Done refreshing spotify token");
    }

    // Obtains the (refreshed) token for the default spotify account
    std::string get_spotify_token() {
        bot->core->log(dpp::ll_debug, "Attempting to retrieve spotify token.");
        std::string token;
        pqxx::work tx(bot->conn);
        try {
            bot->core->log(dpp::ll_debug, fmt::format("Default spotify account: {}", this->defaultSpotifyAccount));
            auto res = tx.exec_params1("SELECT spotify_username,spotify_token,spotify_token_expires,spotify_refresh_token FROM spotify WHERE id=$1", atoi(this->defaultSpotifyAccount.c_str()));
            tx.commit();
            bot->core->log(dpp::ll_trace, "Retrieved from DB.");

            auto ts = res[2].as<asdf::timestamp>();

            if (ts < std::chrono::system_clock::now()) {
                refreshSpotify(res[3].as<std::string>());
                pqxx::work tx(bot->conn);
                res = tx.exec_params1("SELECT spotify_username, spotify_token FROM spotify WHERE id=$1", atoi(this->defaultSpotifyAccount.c_str()));
                tx.commit();
                bot->core->log(dpp::ll_trace, "Retrieved from database *again*");
            }

            token = res[1].as<std::string>(); 
            tx.commit();
        } catch (std::exception &e) {
            std::string error_msg = "Error getting spotify token: " + *e.what();
            bot->core->log(dpp::ll_error, e.what());
            sentry_value_t event = sentry_value_new_event();
            sentry_value_t exc = sentry_value_new_exception("Exception", "Spotify SQL error");
            sentry_value_set_stacktrace(exc, NULL, 5);
            sentry_event_add_exception(event, exc);
            sentry_capture_event(event);
            tx.abort();
            return nullptr;
        }
        bot->core->log(dpp::ll_debug, "Done retrieving spotify token.");

        return token;
    }

    /**
     * Performs a GET request to the Spotify API
    */
    json spotify_get(const std::string route) {
        bot->core->log(dpp::ll_debug, fmt::format("Making GET request to {}/{}", this->spotifyBaseUrl, route));

        std::string token = get_spotify_token();
        bot->core->log(dpp::ll_trace, "Obtained token.");

        cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/{}", this->spotifyBaseUrl, route).c_str()},
            cpr::Bearer{token});
        
        bot->core->log(dpp::ll_trace, fmt::format("Made request. Code: {}", r.status_code));

        if (r.status_code != 200) {
            bot->core->log(dpp::ll_error, fmt::format("Spotify API Error: {}", r.text));
            throw std::exception();
        }

        return json::parse(r.text);
    }

    void spotify_post(const std::string route) {
        return spotify_post(route, 200);
    }

    /**
     * Performs a POST request to the Spotify API
    */
   void spotify_post(const std::string route, int expected_code) {
        bot->core->log(dpp::ll_debug, fmt::format("Making spotify POST to {}", route));
        std::string token = get_spotify_token();
        bot->core->log(dpp::ll_trace, "Obtained Token.");

        cpr::Response r = cpr::Post(cpr::Url{fmt::format("{}/{}", this->spotifyBaseUrl, route).c_str()},
            cpr::Bearer{token});
        bot->core->log(dpp::ll_trace, fmt::format("Made request. Code: {}", r.status_code));

        if (r.status_code != expected_code) {
            bot->core->log(dpp::ll_error, fmt::format("Spotify API Error: {}", r.text));
            throw std::exception();
        }

        return;
   }


    virtual bool OnMessage(const dpp::message_create_t &message, const std::string& clean_message, bool mentioned, const std::vector<std::string> & mentions) {
        sentry_set_tag("module", "spotify");
        bot->core->log(dpp::ll_debug, "Got message event");        

        std::regex re("^https:\/\/open.spotify.com\/track\/([a-zA-Z0-9]+)(.*)$");

        std::smatch match;

        if (std::regex_search(clean_message, match, re) == true) {
            try {
                json res = spotify_get("tracks/" + match.str(1));

                std::string post_rt = fmt::format("me/player/queue?uri=spotify:track:{}{}", match.str(1), this->spotifyDefaultDevice != "" ? "&device_id=" + this->spotifyDefaultDevice : "");                

                spotify_post(post_rt, 204);
        
                dpp::embed embed = dpp::embed()
                    .set_title(res["name"])
                    .set_author(res["artists"][0]["name"], res["artists"][0]["external_urls"]["spotify"], "")
                    .set_thumbnail(res["album"]["images"][0]["url"])
                    .set_description("Added to the queue!");

                bot->core->message_create(dpp::message(message.msg.channel_id, embed).set_reference(message.msg.id));
                return true;
            } catch (std::exception &e) {
                EmbedError(message.msg.channel_id, e);
                return false;
            }
        } 

        return true;
    }

    bool OnCommand(const dpp::message_create_t &message, const std::string &command, const std::vector<std::string>& params) {
        
        if (command == "spotify") {
            // We can process subcommands!

            if (params.size() <= 1) {
                return true; // we don't have enough to process
            }

            std::string subcommand = lowercase(params[1]);

            if (subcommand == "accounts") {
                pqxx::work tx(bot->conn);

                auto res = tx.exec("SELECT id, spotify_username FROM spotify");
                tx.commit();
                dpp::embed embed = dpp::embed().
                    set_color(dpp::colors::green)
                    .set_title("Spotify Accounts")
                    .set_description("List of Spotify accounts. Username is field ID and ID is field content.\n\nCurrent default account ID: " + this->defaultSpotifyAccount);

                for (int i = 0; i < res.size(); i++) {
                    embed.add_field(res[i][1].c_str(), res[i][0].c_str(), true);
                }


                bot->core->message_create(dpp::message(message.msg.channel_id, embed).set_reference(message.msg.id));
                return true;
            } else if (subcommand == "account") {
                pqxx::work tx(bot->conn);

                try {
                    auto res = tx.exec_params1("SELECT id FROM spotify WHERE id=$1", params[2]);

                    this->defaultSpotifyAccount = params[2];
                    tx.exec_prepared("update_state", "default_spotify_account", params[2]);

                    tx.commit();
                } catch (std::exception &e) {
                    bot->core->log(dpp::ll_error, e.what());
                    sentry_value_t event = sentry_value_new_event();
                    sentry_value_t exc = sentry_value_new_exception("Exception", "Spotify SQL error");
                    sentry_value_set_stacktrace(exc, NULL, 5);
                    sentry_event_add_exception(event, exc);
                    sentry_capture_event(event);
                    tx.abort();
                    EmbedError("Invalid ID", message.msg.channel_id); 
                    return false;
                }

                EmbedSuccess("Updated default account.", message.msg.channel_id);
            }else if (subcommand == "devices") {
                json res = spotify_get("me/player/devices");

                auto devices = res["devices"];
                dpp::embed embed = dpp::embed().
                    set_color(dpp::colors::green)
                    .set_title("Spotify Devices")
                    .set_description("List of Spotify devices. Username is field ID and ID is field content.\n\nCurrent default account ID: " + this->defaultSpotifyAccount);


                for (int i = 0; i < devices.size(); i++) {
                    std::string name = fmt::format("{} ({})", devices[i]["name"], devices[i]["id"].get<std::string>());
                    std::string content = fmt::format("{} {}", devices[i]["type"].get<std::string>(), devices[i]["id"].get<std::string>() == this->spotifyDefaultDevice ? "(Default)" : "");
                    embed.add_field(name, content, true);
                }
                bot->core->message_create(dpp::message(message.msg.channel_id, embed).set_reference(message.msg.id));

            } else if (subcommand == "device") {
                json res = spotify_get("me/player/devices");

                auto devices = res["devices"];

                for (int i = 0; i < devices.size(); i++) {
                    if (devices[i]["id"] == params[2]) {
                        this->spotifyDefaultDevice = params[2];
                        pqxx::work tx(bot->conn);
                        tx.exec_prepared("update_state", "default_spotify_device", params[2]);
                        tx.commit();
                        EmbedSuccess("Changed default spotify device", message.msg.channel_id);
                        return true;
                    }
                }

                EmbedError("Invalid ID", message.msg.channel_id);
            } else if (subcommand == "status") {
                auto res = spotify_get("me/player");

                auto item = res["item"];
                auto device = res["device"];
                auto album = item["album"];

                dpp::embed embed = dpp::embed()
                    .set_author(item["artists"][0]["name"], item["artists"][0]["external_urls"]["spotify"], "")
                    .set_title(fmt::format("\"{}\" on {} by {}", item["name"], album["name"], album["artists"][0]["name"]))
                    .set_thumbnail(album["images"][0]["url"])
                    .add_field("Status", res["is_playing"].get<bool>() ? "Playing" : "Paused", true)
                    .add_field("Repeating?", res["repeat_state"], true);

                bot->core->message_create(dpp::message(message.msg.channel_id, embed).set_reference(message.msg.id));
            } else {
                EmbedError("Unknown Command", message.msg.channel_id);
            }
        }

        return true;
    }
};



ENTRYPOINT(SpotifyModule)
