#include "genginepch.h"
#include "GEngineLog.h"

std::shared_ptr<spdlog::logger> GEngineLog::s_ClientLogger;

void GEngineLog::Init()
{
	spdlog::set_pattern("%^[%T] %@ %n: %v%$");
	s_ClientLogger = spdlog::stdout_color_mt("GENGINE");
	s_ClientLogger->set_level(spdlog::level::trace);
}