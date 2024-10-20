#include "genginepch.h"
#include "GEngineProject.h"

GEngineProject::GEngineProject()
{	
	m_SceneManager = new GEngineSceneManager();
	m_SceneManager->Init();
}