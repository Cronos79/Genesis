#include "genginepch.h"
#include "GEngineProject.h"
#include "GEngineProjectSettings.h"

GEngineProject::GEngineProject()
{	
	m_SceneManager = new GEngineSceneManager();
	m_SceneManager->Init();
	m_ProjectSettings = new GEngineProjectSettings();
}