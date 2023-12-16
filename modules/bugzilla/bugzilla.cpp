#include <142bot/modules.hpp>
#include "cpr/cpr.h"
#include <fmt/format.h>
#include <cpr/response.h>
#include <dpp/dispatcher.h>
#include <dpp/dpp.h>
#include <regex>
#include <pcre.h>

using json = nlohmann::json;

class BugzillaModule: public Module {
  std::string bugzillaBaseUrl;
  std::string bugzillaApiKey;
public:
  BugzillaModule(Bot* creator, ModuleLoader* ml): Module(creator, ml) {
    ml->attach({I_OnMessage}, this);

    this->bugzillaBaseUrl = creator->cfg.value("bugzilla_base", "https://bugzilla.example.com");
    this->bugzillaApiKey = creator->cfg.value("bugzilla_api_key", "bad-key");
  }

  virtual std::string version() {
    return "0.1.0";
  }
  virtual std::string description() {
    return "Work with bugzilla";
  }


  json bugzilla_get(const std::string route) {
    bot->core->log(dpp::ll_debug, fmt::format("Bugzilla: making GET request to {}/{}", this->bugzillaBaseUrl, route));

    cpr::Response r = cpr::Get(cpr::Url{fmt::format("{}/{}&api_key={}", this->bugzillaBaseUrl, route, this->bugzillaApiKey).c_str()},
			       cpr::Header{"Content-Type", "application/json"});

    bot->core->log(dpp::ll_trace, fmt::format("Bugzilla: made request. Code: {}", r.status_code));

    if (r.status_code != 200) {
      bot->core->log(dpp::ll_error, fmt::format("Bugzilla API error: {}", r.text));
      throw std::exception();
    }

    return json::parse(r.text);
  }



  virtual bool OnMessage(const dpp::message_create_t &message, const std::string &clean_message, bool mentioned, const std::vector<std::string> &mentions) {
    // check to see if a bug was mentioned
    std::regex re("bug ([0-9]*)");
    std::smatch match;

    if (std::regex_search(clean_message, match, re) == true) {
      // we found a bug thing!
      bot->core->message_create(dpp::message(message.msg.channel_id, "Found a bug! " + match.str(1)));

      json res = bugzilla_get(fmt::format("/bugs?id={}", match.str(1)));
      bot->core->log(dpp::ll_debug, res);
      
      return true;
    }

    return true;
  }

  
};

ENTRYPOINT(BugzillaModule)
