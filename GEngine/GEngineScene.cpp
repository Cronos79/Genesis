#include "genginepch.h"
#include "GEngineScene.h"
#include "GEngineGameObject.h"
#include "GEngineImGuiObject.h"

GEngineScene::GEngineScene()
{

}

GEngineScene::~GEngineScene()
{

}

void GEngineScene::PushGameObject(GEngineGameObject* object)
{
	m_GameEngineStack.PushGameObject(object);
	object->OnAttach();
}

void GEngineScene::PushOverlay(GEngineImGuiObject* object)
{
	m_GameEngineStack.PushOverlay(object);
	object->OnAttach();
}

GEngineGameObjectStack GEngineScene::GetGameObjects()
{
	return m_GameEngineStack;
}
