#pragma once
#include "epch.h"

#include <spdlog/spdlog.h>

namespace Skeleton {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

// Log macros
#define LOG_CRITICAL(...)      ::Skeleton::Log::GetLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...)         ::Skeleton::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)          ::Skeleton::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)          ::Skeleton::Log::GetLogger()->info(__VA_ARGS__)