#include <142bot/modules.hpp>
#include "cpr/cpr.h"

using json = nlohmann::json;

class BugzillaModule: public Module {
  std::string bugzillaBaseUrl;
public:
  BugzillaModule(Bot* creator, ModuleLoader* ml): Module(creator, ml) {
    ml->attach({I_OnMessage}, this);
  }

  virtual std::string version() {
    return "0.1.0";
  }
  virtual std::string description() {
    return "Work with bugzilla";
  }

  
};

ENTRYPOINT(BugzillaModule)
