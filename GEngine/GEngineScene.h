#pragma once
#include "GEngineGameObjectStack.h"

class GEngineGameObject;
class GEngineImGuiObject;
class GEngineScene
{
public:
	GEngineScene();
	~GEngineScene();

	void PushGameObject(GEngineGameObject* object);
	void PushOverlay(GEngineImGuiObject* object);

	GEngineGameObjectStack GetGameObjects();

private:
	GEngineGameObjectStack m_GameEngineStack;
};

