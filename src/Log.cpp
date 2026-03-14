#include "epch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace Skeleton {

	std::shared_ptr<spdlog::logger> Log::s_Logger;

	void Log::Init()
	{
		// Disable the global default pattern
		spdlog::set_pattern("%v");
		
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_pattern("%v");
		
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/server.log", true);
		file_sink->set_pattern("%v");
		
		s_Logger = std::make_shared<spdlog::logger>("SKELETON", spdlog::sinks_init_list{console_sink, file_sink});
		s_Logger->set_level(spdlog::level::trace);
		spdlog::register_logger(s_Logger);
		spdlog::set_default_logger(s_Logger);
	}
}
