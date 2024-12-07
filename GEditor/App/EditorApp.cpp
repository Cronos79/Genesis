/******************************************************************************************
*	CronoGames Game Engine																  *
*	Copyright © 2024 CronoGames <http://www.cronogames.net>								  *
*																						  *
*	This file is part of CronoGames Game Engine.										  *
*																						  *
*	CronoGames Game Engine is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The CronoGames Game Engine is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The CronoGames Game Engine.  If not, see <http://www.gnu.org/licenses/>.   *
******************************************************************************************/
#include "EditorApp.h"
#include "Gengine/Win/EntryPoint.cpp"
#include "GEngine/Core/GContext.h"
#include "GEngine/Win/Window.h"
#include "../Project/ProjectMng.h"

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
		GContext::Get().Init(1920, 1080, "Genesis Editor");	
		
	}

	void EditorApp::HandleInput(float deltaTime)
	{
		// Raw mouse input event #TODO: Needs a lot of work
	/*	if (GContext::Get().GetWindow()->mouse.RightIsPressed())
		{
			if (auto md = GContext::Get().GetWindow()->mouse.ReadRawDelta())
			{
				GContext::Get().GetGraphics()->GetDX11()->m_Camera.AdjustRotation((float)md->y * 0.001f, (float)md->x * 0.001f, 0.0f);
			}
		}*/

		const float cameraSpeed = m_ui.m_CameraSpeed;
		if (GContext::Get().GetWindow()->kbd.KeyIsPressed('W'))
		{
			GContext::Get().GetGraphics()->GetDX11()->m_Camera.AdjustPosition(GContext::Get().GetGraphics()->GetDX11()->m_Camera.GetForwardVector() * cameraSpeed * deltaTime);
		}
		if (GContext::Get().GetWindow()->kbd.KeyIsPressed('S'))
		{
			GContext::Get().GetGraphics()->GetDX11()->m_Camera.AdjustPosition(GContext::Get().GetGraphics()->GetDX11()->m_Camera.GetBackwardVector() * cameraSpeed * deltaTime);
		}
		if (GContext::Get().GetWindow()->kbd.KeyIsPressed('A'))
		{
			GContext::Get().GetGraphics()->GetDX11()->m_Camera.AdjustPosition(GContext::Get().GetGraphics()->GetDX11()->m_Camera.GetLeftVector() * cameraSpeed * deltaTime);
		}
		if (GContext::Get().GetWindow()->kbd.KeyIsPressed('D'))
		{
			GContext::Get().GetGraphics()->GetDX11()->m_Camera.AdjustPosition(GContext::Get().GetGraphics()->GetDX11()->m_Camera.GetRightVector() * cameraSpeed * deltaTime);
		}
		if (GContext::Get().GetWindow()->kbd.KeyIsPressed('Q'))
		{
			GContext::Get().GetGraphics()->GetDX11()->m_Camera.AdjustPosition(0.0f, -cameraSpeed * deltaTime, 0.0f);
		}
		if (GContext::Get().GetWindow()->kbd.KeyIsPressed('E'))
		{
			GContext::Get().GetGraphics()->GetDX11()->m_Camera.AdjustPosition(0.0f, cameraSpeed * deltaTime, 0.0f);
		}

		// Toggle Vsync
		if (GContext::Get().GetWindow()->kbd.KeyIsPressed('V'))
		{
			GContext::Get().GetGraphics()->GetDX11()->ToggleVsync();
		}
	}

	void EditorApp::Update(float deltaTime)
	{
		// Update the main UI
		m_ui.Draw(deltaTime);
	}

	void EditorApp::Shutdown()
	{
		
	}

}

Genesis::EngineApp* CreateEngineApp()
{
	return new Genesis::EditorApp();
}