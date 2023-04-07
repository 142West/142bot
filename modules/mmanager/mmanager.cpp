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
#include <142bot/util.hpp>
#include <pcre.h>
#include <dpp/dpp.h>
#include <fmt/format.h>
using namespace fmt;

class MManagerModule : public Module
{
    double microseconds_ping;

public:
    MManagerModule(Bot *creator, ModuleLoader *ml) : Module(creator, ml)
    {
        ml->attach({I_OnMessage, I_OnReady}, this);
        creator->core->log(dpp::ll_info, "ModuleManager online!");
    }

    virtual std::string version()
    {
        return "0.1.0";
    }

    virtual std::string description()
    {
        return "module manager";
    }

    virtual bool OnReady(const dpp::ready_t &ready)
    {
        bot->core->log(dpp::ll_info, "Got ready event");
        return true;
    }

    virtual bool OnMessage(const dpp::message_create_t &message, const std::string &clean_message, bool mentioned, const std::vector<std::string> &stringmentions)
    {
       dpp::message msg = message.msg;
        sentry_set_tag("module", "mmanager");
        std::vector<std::string> param;
        const char *pcre_error;
        int pcre_error_ofs;
        auto comp = pcre_compile(std::string("^sudo(\\s+(.+?))$").c_str(), PCRE_CASELESS | PCRE_MULTILINE, &pcre_error, &pcre_error_ofs, NULL);
        if (!comp)
        {
            bot->core->log(dpp::ll_error, pcre_error);
        }

        int matcharr[90];
        int matchcount = pcre_exec(comp, NULL, clean_message.c_str(), clean_message.length(), 0, 0, matcharr, 90);
        for (int i = 0; i < matchcount; ++i)
        {
            param.push_back(std::string(clean_message.c_str() + matcharr[2 * i], (size_t)(matcharr[2 * i + 1] - matcharr[2 * i])));
        }
        if (mentioned && matchcount > 0)
        {
            if (message.msg.author.id == bot->get_owner_id())
            {
                // Tokenize
                for (int i = 0; i < param.size(); i++)
                {
                    bot->core->log(dpp::ll_debug, fmt::format("{}", param[i]));
                }
                std::stringstream tokens(trim(param[2]));
                std::string subcommand;
                tokens >> subcommand;

                bot->core->log(dpp::ll_warning, fmt::format("SUDO: <{}> {}", message.msg.author.username, clean_message));
        sentry_value_t crumb = sentry_value_new_breadcrumb("user", "got sudo command");
        sentry_value_set_by_key(crumb, "level", sentry_value_new_string("warning"));
        sentry_value_set_by_key(crumb, "category", sentry_value_new_string("sudo"));
        sentry_value_set_by_key(crumb, "data", sentry_value_new_string(subcommand.c_str()));
        sentry_add_breadcrumb(crumb);

                if (lowercase(subcommand) == "modules")
                {
                    std::stringstream s;

                    // NOTE: GetModuleList's reference is safe from within a module event
                    const ModuleMap &modlist = bot->loader->get_loaded_modules();

                    s << "```diff" << std::endl;
                    s << fmt::format("╭─────────────────────────┬───────────┬────────────────────────────────────────────────╮") << std::endl;
                    s << fmt::format("│ Filename                | Version   | Description                                    |") << std::endl;
                    s << fmt::format("├─────────────────────────┼───────────┼────────────────────────────────────────────────┤") << std::endl;

                    for (auto mod = modlist.begin(); mod != modlist.end(); ++mod)
                    {
                        s << fmt::format("│ {:23} | {:9} | {:46} |", mod->first, mod->second->version(), mod->second->description()) << std::endl;
                    }
                    s << fmt::format("╰─────────────────────────┴───────────┴────────────────────────────────────────────────╯") << std::endl;
                    s << "```";

                    dpp::channel *c = dpp::find_channel(message.msg.channel_id);
                    if (c)
                    {
                        bot->core->message_create(dpp::message(c->id, s.str()));
                    }
                }
                else if (lowercase(subcommand) == "load")
                {
                    std::string modfile;
                    tokens >> modfile;
                    if (bot->loader->load(modfile))
                    {
                        EmbedSimple("Loaded module: " + modfile, message.msg.channel_id);
                    }
                    else
                    {
                        EmbedSimple(std::string("Can't do that, check server logs"), message.msg.channel_id);
                    }
                }
                else if (lowercase(subcommand) == "unload")
                {
                    std::string modfile;
                    tokens >> modfile;
                    if (modfile == "module_mmanager.so")
                    {
                        EmbedSimple("That's the module manager, are you sure about that chief?", message.msg.channel_id);
                    }

                    if (bot->loader->unload(modfile))
                    {
                        EmbedSimple("Unloaded module: " + modfile, message.msg.channel_id);
                    }
                    else
                    {
                        EmbedSimple("Can't do that, check server logs.", message.msg.channel_id);
                    }
                }
                else if (lowercase(subcommand) == "reload")
                {
                    std::string modfile;
                    tokens >> modfile;
                    if (modfile == "module_mmanager.so")
                    {
                        EmbedSimple("That's the module manager, are you sure about that chief?", message.msg.channel_id);
                        ::sleep(500);
                    }

                    if (bot->loader->reload(modfile))
                    {
                        EmbedSimple("Reloaded module: " + modfile, message.msg.channel_id);
                    }
                    else
                    {
                        EmbedSimple("Can't do that, check server logs", message.msg.channel_id);
                    }
                }
                else if (lowercase(subcommand) == "ping")
                {
                    dpp::channel *c = dpp::find_channel(message.msg.channel_id);
                    if (c)
                    {
                        std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
                        dpp::snowflake cid = message.msg.channel_id;
                        bot->core->message_create(dpp::message(message.msg.channel_id, "Pinging..."), [cid, this, start_time](const dpp::confirmation_callback_t &state)
                                                  {
								double microseconds_ping = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time).count();
								dpp::snowflake mid = (std::get<dpp::message>(state.value)).id;
								this->bot->core->message_delete(mid, cid);
								this->EmbedSimple(fmt::format("**Pong!** REST Response time: {:.3f} ms", microseconds_ping / 1000, 4), cid); });
                    }
                }
                else if (lowercase(subcommand) == "restart")
                {
                    EmbedSimple("Restarting...", message.msg.channel_id);
                    ::sleep(5);
                    exit(0);
                }
                else if (lowercase(subcommand) == "sql")
                {
                    std::string sql;
                    std::getline(tokens, sql);
                    sql = trim(sql);
                    // Create transaction
                    pqxx::work tx(bot->conn); 

                    try {
                        auto res = tx.exec(sql);
                    
                    std::stringstream w;
                 
                    for (size_t i = 0; i < res.size(); i++) {
                        bot->core->log(dpp::ll_debug, res[i][0].c_str());
                    }
                    if (res.size() == 0)
                    {
                        {
                            EmbedSimple("Successfully executed, no rows returned.", msg.channel_id);
                        }
                    }
                    else
                    {
                        w << "- " << sql << std::endl; 
                        auto check = res.begin();
                        w << "+ Rows Returned: " << res.size() << std::endl;
                        for (auto row = res.begin(); row != res.end(); ++row)
                        {
                            if (row == res.begin())
                            {
                                w << "  ╭";
                            }
                            w << "────────────────────";
                            check = row;
                            w << (++check != res.end() ? "┬" : "╮\n");
                        }
                        w << "  ";
                        for (auto row = res.begin(); row != res.end(); ++row)
                        {
                            w << fmt::format("│{:20}", row[0].c_str());
                        }
                        w << "│" << std::endl;
                        for (auto row = res.begin(); row != res.end(); ++row)
                        {
                            if (row == res.begin())
                            {
                                w << "  ├";
                            }
                            w << "────────────────────";
                            check = row;
                            w << (++check != res.end() ? "┼" : "┤\n");
                        }
                        for (auto row : res)
                        {
                            if (w.str().length() < 1900)
                            {
                                w << "  ";
                                for (auto field : row)
                                {
                                    w << fmt::format("│{:20}", field.as<std::string>().substr(0, 20));
                                }
                                w << "│" << std::endl;
                            }
                        }
                        for (auto row = res.begin(); row != res.end(); ++row)
                        {
                            if (row == res.begin())
                            {
                                w << "  ╰";
                            }
                            w << "────────────────────";
                            check = row;
                            w << (++check != res.end() ? "┴" : "╯\n");
                        }
                        dpp::channel *c = dpp::find_channel(msg.channel_id);
                        if (c)
                        {
                                bot->core->message_create(dpp::message(msg.channel_id, "```diff\n" + w.str() + "```"));
                        }
                    }
                    tx.commit();
                } catch (std::exception &e) {
                        std::string error_msg = fmt::format("Error in SQL statement: {}", e.what());
                        bot->core->log(dpp::ll_error, error_msg);
                        EmbedSimple("Error in SQL statement, check logs", msg.channel_id);
                        sentry_value_t event = sentry_value_new_event();
                        sentry_value_t exc = sentry_value_new_exception("Exception", "Custom SQL error");
                        sentry_value_set_stacktrace(exc, NULL, 5);
                        sentry_event_add_exception(event, exc);
                        sentry_capture_event(event);
                        tx.abort();
                        return false;
                    
                } 
                }
                else
                {
                    EmbedSimple("Command not found.", message.msg.channel_id);
                }
            }
            else
            {
                bot->core->log(dpp::ll_error, fmt::format("Called ModuleManager as a mortal ({})", bot->get_owner_id()));
                message.reply(dpp::message("nope"));
            }
        }

        return true;
    }
};

ENTRYPOINT(MManagerModule)
