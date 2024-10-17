#pragma once
// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#pragma warning(pop)

class GEngineLog
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

std::shared_ptr<spdlog::logger> GEngineLog::s_ClientLogger;

void GEngineLog::Init()
{
	spdlog::set_pattern("%^[%T] %@ %n: %v%$");
	s_ClientLogger = spdlog::stdout_color_mt("GENGINE");
	s_ClientLogger->set_level(spdlog::level::trace);
}

#ifdef _DEBUG
#define BB_TRACE(...)         ::GEngineLog::GetClientLogger()->trace(__VA_ARGS__)
#define BB_INFO(...)          ::GEngineLog::GetClientLogger()->info(__VA_ARGS__)
#define BB_WARN(...)          ::GEngineLog::GetClientLogger()->warn(__VA_ARGS__)
#define BB_ERROR(...)         ::GEngineLog::GetClientLogger()->error(__VA_ARGS__)
#define BB_CRITICAL(...)      ::GEngineLog::GetClientLogger()->critical(__VA_ARGS__)
#else
#define BB_TRACE
#define BB_INFO
#define BB_WARN
#define BB_ERROR
#define BB_CRITICAL
#endif