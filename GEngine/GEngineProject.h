#pragma once
#include "GEngineSceneManager.h"

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
};

