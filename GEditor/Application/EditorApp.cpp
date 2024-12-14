#include "EditorApp.h"
#include "GEngine/src/Entry/EntryPoint.cpp"

namespace Genesis
{

	EditorApp::EditorApp()
	{

	}

	EditorApp::~EditorApp()
	{

	}

	void EditorApp::Init()
	{
		
	}

	void EditorApp::HandleInput(float deltaTime)
	{

	}

	void EditorApp::Update(float deltaTime)
	{

	}

	void EditorApp::Shutdown()
	{

	}
}

Genesis::EngineApp* CreateEngineApp()
{
	return new Genesis::EditorApp();
}