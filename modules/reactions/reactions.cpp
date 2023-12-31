#include <dpp/message.h>
#include <dpp/json.h>
#include <stdlib.h>
#include <142bot/modules.hpp>
#include <142bot/util.hpp>
#include <fmt/format.h>
#include <sentry.h>

using std::to_string;
class ReactionsModule : public Module {
    std::map<std::string, std::string> reactionMap;
public:
    ReactionsModule(Bot* creator, ModuleLoader* ml): Module(creator, ml) {
        ml->attach({I_OnMessage}, this);

        sentry_value_t crumb = sentry_value_new_breadcrumb("default", "Loaded module reactions");
        sentry_add_breadcrumb(crumb);

        std::ifstream f("resources/reactions.json");
        json reactions = json::parse(f);

        for(auto it = reactions.begin(); it != reactions.end(); it++) {
            reactionMap.insert_or_assign(it.key(), reactions.value(it.key(), ""));
        }
    }

    virtual std::string version() {
        return "0.1.0";
    }

    virtual std::string description() {
        return "Auto-reactions based on keyword";
    }
	virtual bool OnMessage(const dpp::message_create_t &message, const std::string& clean_message, bool mentioned, const std::vector<std::string> &stringmentions) {
        sentry_set_tag("module", "reactions");
        for (auto i = reactionMap.begin(); i != reactionMap.end(); i++) {
            if (lowercase(clean_message).find(i->first) != std::string::npos) {
                bot->core->message_add_reaction(message.msg, i->second);
                bot->core->log(dpp::ll_debug, "Adding reaction based on keyword");
            }
        }

        return true;
    }
    
};


ENTRYPOINT(ReactionsModule)