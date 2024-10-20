#include "genginepch.h"
#include "GEngineProjectMng.h"

#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <nlohmann/json.hpp>
// https://json.nlohmann.me/features/arbitrary_types/#can-i-write-my-own-serializer-advanced-use
// for convenience
using json = nlohmann::json;

#include "GEngineContext.h"
#include "GEngineLog.h"

GEngineProjectMng::GEngineProjectMng()
{
	m_CurrentProject = new GEngineProject();
}

std::filesystem::path GEngineProjectMng::CreateProjectPaths(ProjectData data)
{
	
	fs::path projectDir = data.ProjectName;
	fs::path projectsPath = GetProjectsPath();
	if (!fs::exists(projectsPath))
	{
		fs::create_directory(projectsPath);
	}
	fs::path fullPath = projectsPath / projectDir;
	if (!fs::exists(fullPath))
	{
		fs::create_directory(fullPath);
	}
	fs::path content = "Content";
	fs::path contentPath = fullPath / content;
	if (!fs::exists(contentPath))
	{
		fs::create_directory(contentPath);
	}
	fs::path scenes = "Scenes";
	fs::path scenesPath = contentPath / scenes;
	if (!fs::exists(scenesPath))
	{
		fs::create_directory(scenesPath);
	}

	return fullPath;
}

std::vector<GEngineProject*> GEngineProjectMng::GetAllProjects()
{
	std::vector<GEngineProject*> p;

	fs::path projectsPath = GetProjectsPath();
	for (const auto& entry : fs::directory_iterator(projectsPath))
	{
		std::string dirPath = entry.path().string();
		std::string dir = "";

		int last = dirPath.find_last_of('\\');
		dir = dirPath.erase(0, last + 1);
		ProjectData data = GEngineContext::GetInstance().GetProjectMng()->GELoadProject(dir);
		GEngineProject* proj = new GEngineProject();
		proj->m_Data = data;
		p.push_back(proj);
	}
	return p;
}

GEngineProject* GEngineProjectMng::GetCurrentProject()
{
	return m_CurrentProject;
}

void GEngineProjectMng::SetCurrentProject(GEngineProject* project)
{
	// #TODO: Load project... just not ProjectData
	BB_INFO(std::format("Loaded: {}", project->m_Data.ProjectName));
	m_CurrentProject = project;
}

void GEngineProjectMng::GESaveProject(ProjectData data)
{
	data.EngineVersion = GEngineContext::GetInstance().GetVersion().ToString();
	std::string fileName = data.ProjectName + ".gproj";
	json j;
	j["ProjectName"] = data.ProjectName;
	j["EngineVersion"] = data.EngineVersion;
	j["Description"] = data.Description;
	std::string jsontxt = j.dump();

	fs::path fullPath = CreateProjectPaths(data);

	std::string fullFileName = fullPath.string() + "\\" + fileName;
	std::ofstream myfile(fullFileName);
	if (myfile.is_open())
	{
		myfile << jsontxt;
		myfile.close();
	}
}

ProjectData GEngineProjectMng::GELoadProject(std::string projectName)
{
	std::string fileName = projectName + ".gproj";
	std::string line;
	std::string jsontxt;

	fs::path projectsDir = GetProjectsPath();
	fs::path projectDir = projectName;
	fs::path fullPath = projectsDir / projectDir;

	std::string fullFileName = fullPath.string() + "\\" + fileName;

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
	ProjectData pd = {};
	pd.ProjectName = j2["ProjectName"];
	pd.EngineVersion = j2["EngineVersion"];
	pd.Description = j2["Description"];
	return pd;
}

std::filesystem::path GEngineProjectMng::GetProjectsPath()
{
	fs::path exeDir = fs::current_path();
	fs::path projectsDir = "Projects";
	fs::path projectsPath = exeDir / projectsDir;
	return projectsPath;
}
