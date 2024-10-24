#pragma once
#include "GEngineSceneManager.h"

class GEngineProjectSettings;

struct ProjectData
{
	std::string ProjectName;
	std::string EngineVersion;
	std::string Description;
};

class GEngineProject
{
public:
	GEngineProject();

	ProjectData m_Data;
	GEngineSceneManager* m_SceneManager;
	GEngineProjectSettings* m_ProjectSettings;
};

