#pragma once
#include "GEngineScene.h"
class GEngineSceneManager
{
public:
	GEngineSceneManager();
	~GEngineSceneManager();

	void Init();

	GEngineScene* GetCurrentScene();

private:
	GEngineScene* m_CurrentScene;
};

