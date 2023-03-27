#include <dpp/dpp.h>
#include "include/json.hpp"

using namespace std;

using json = nlohmann::json;

int main(int argc, char const *argv[]) {
    std::ifstream f("config.json");
    json cfg = json::parse(f);
    string token = cfg.value("token", "bad-token");

    dpp::cluster bot(token, dpp::intents::i_all_intents);

    bot.on_log(dpp::utility::cout_logger());
    /* code */

    bot.on_slashcommand([](const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
    });

    bot.on_ready([&bot](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("ping", "Ping!", bot.me.id));
        }
    });

    bot.start(dpp::st_wait);
    return 0;
}
