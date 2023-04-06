#include <dpp/dpp.h>
#include <dpp/json.h>
#include <142bot/bot.hpp>
#include <142bot/db.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <string>

using namespace std;

using json = nlohmann::json;

int main(int argc, char const *argv[]) {
    
    std::ifstream f("config.json");
    json cfg = json::parse(f);
    string token = cfg.value("token", "bad-token");
    dpp::cluster bot(token, dpp::intents::i_all_intents);

		std::shared_ptr<spdlog::logger> log;
		spdlog::init_thread_pool(8192, 2);
		std::vector<spdlog::sink_ptr> sinks;
		auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
		auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("142bot.log", 1024 * 1024 * 5, 10);
		sinks.push_back(stdout_sink);
		sinks.push_back(rotating);
		log = std::make_shared<spdlog::async_logger>("logs", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
		spdlog::register_logger(log);
		log->set_pattern("%^%Y-%m-%d %H:%M:%S.%e [%L] [th#%t]%$ : %v");
		log->set_level(spdlog::level::level_enum::trace);	

		/* Integrate spdlog logger to D++ log events */
		bot.on_log([&bot, &log](const dpp::log_t & event) {
			switch (event.severity) {
				case dpp::ll_trace:
					log->trace("{}", event.message);
				break;
				case dpp::ll_debug:
					log->debug("{}", event.message);
				break;
				case dpp::ll_info:
					log->info("{}", event.message);
				break;
				case dpp::ll_warning:
					log->warn("{}", event.message);
				break;
				case dpp::ll_error:
					log->error("{}", event.message);
				break;
				case dpp::ll_critical:
				default:
					log->critical("{}", event.message);
				break;
			}
		});
    /* code */

    Bot client(0, &bot);

    client.set_owner_id(dpp::snowflake(cfg.value("owner", "00000000000")));

    		/* Attach events to the Bot class methods */
		bot.on_message_create(std::bind(&Bot::onMessage, &client, std::placeholders::_1));
		bot.on_ready(std::bind(&Bot::onReady, &client, std::placeholders::_1));
		bot.on_message_reaction_add(std::bind(&Bot::onMessageReactionAdd, &client, std::placeholders::_1));

    bot.start(dpp::st_wait);
    return 0;
}
