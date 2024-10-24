#pragma once
#include "GEngineProject.h"
#include <filesystem>

class GEngineProjectMng
{
public:
	GEngineProjectMng();

	std::filesystem::path CreateProjectPaths(ProjectData data);

	std::vector<GEngineProject*> GetAllProjects();

	GEngineProject* GetCurrentProject();
	void SetCurrentProject(GEngineProject* project);

	void GESaveProject(ProjectData data);
	ProjectData GELoadProject(std::string name);

	void Init();
	void ShutDown();

private:
	std::filesystem::path GetProjectsPath();

private:
	GEngineProject* m_CurrentProject;
};

