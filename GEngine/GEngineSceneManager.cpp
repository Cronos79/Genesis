#include "genginepch.h"
#include "GEngineSceneManager.h"

GEngineSceneManager::GEngineSceneManager()
{
	m_CurrentScene = new GEngineScene();
}

GEngineSceneManager::~GEngineSceneManager()
{

}

void GEngineSceneManager::Init()
{
	m_CurrentScene = new GEngineScene();
}

GEngineScene* GEngineSceneManager::GetCurrentScene()
{
	return m_CurrentScene;
}
