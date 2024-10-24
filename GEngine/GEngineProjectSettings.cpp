#include "genginepch.h"
#include "GEngineProjectSettings.h"

#include <iostream>
#include <fstream>
namespace fs = std::filesystem;

#include <nlohmann/json.hpp>
#include "GEngineContext.h"
// https://json.nlohmann.me/features/arbitrary_types/#can-i-write-my-own-serializer-advanced-use
// for convenience
using json = nlohmann::json;

GEngineProjectSettings::GEngineProjectSettings()
{
}

bool GEngineProjectSettings::SaveSettings()
{
	GEngineProject* project = GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject();	

	std::string fileName = "EditorSettings.cfg";
	json j;
	j["DefaultScene"] = m_Data.DefaultScene;


	std::string jsontxt = j.dump();

	fs::path fullPath = GetSettingsPath();

	std::string fullFileName = fullPath.string() + "\\" + fileName;
	std::ofstream myfile(fullFileName);
	if (myfile.is_open())
	{
		myfile << jsontxt;
		myfile.close();
		return true;
	}
	return false;
}

GProjectSettings GEngineProjectSettings::LoadSettings()
{
	GEngineProject* project = GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject();

	std::string fileName = "EditorSettings.cfg";
	std::string line;
	std::string jsontxt;

	fs::path SettingsDir = GetSettingsPath();

	std::string fullFileName = SettingsDir.string() + "\\" + fileName;

	std::ifstream myfile(fullFileName);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			jsontxt += line;
		}
		myfile.close();
	}

	json j2 = json::parse(jsontxt);
	GProjectSettings ps = {};
	ps.DefaultScene = j2["DefaultScene"];
	return ps;
}

std::filesystem::path GEngineProjectSettings::GetSettingsPath()
{
	GEngineProject* project = GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject();
	fs::path exeDir = fs::current_path();
	fs::path projectsDir = "Projects";
	fs::path projectsPath = exeDir / projectsDir;
	fs::path ProjectPath = projectsPath / project->m_Data.ProjectName;
	fs::path SettingsPath = ProjectPath / std::format(".{}Data", project->m_Data.ProjectName);
	return SettingsPath;
}
