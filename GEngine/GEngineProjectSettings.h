#pragma once
#include <string>
#include <filesystem>
#include "GEngineProject.h"

struct GProjectSettings
{
	std::string DefaultScene = "Default";
};

class GEngineProjectSettings
{
public:
	GEngineProjectSettings();

	bool SaveSettings();
	GProjectSettings LoadSettings();

private:
	std::filesystem::path GetSettingsPath();

private:
	GProjectSettings m_Data;
};

