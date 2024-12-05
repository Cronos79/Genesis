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
		//GContext::Get().Init(800, 600, "Genesis Editor");
	}

	void EditorApp::HandleInput(float deltaTime)
	{

	}

	void EditorApp::Update(float deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Begin("FPS");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();		

		ImGui::Begin("Mouse");
		ImGui::Text("Mouse X %.1f Mouse Y %.1f", (float)GContext::Get().GetWindow()->mouse.GetPosX(), (float)GContext::Get().GetWindow()->mouse.GetPosY());
		ImGui::End();
		/*
		float width = (float)GContext::Get().GetWidth() / 2.0f;
		float height = (float)GContext::Get().GetHeight() / 4.0f;
		static float f = 0.0f;
		f += deltaTime;*/
		/*GContext::Get().GetGraphics()->GetDX11Core()->DrawCube(
			f,
			0,
			0,
			0.0f);
		GContext::Get().GetGraphics()->GetDX11Core()->DrawCube(
			f,
			(float)GContext::Get().GetWindow()->mouse.GetPosX() / width - 1.0f,
			0.0f,
			-(float)GContext::Get().GetWindow()->mouse.GetPosY() / height + 2.0f);*/
	}

	void EditorApp::Shutdown()
	{
		
	}

}

Genesis::EngineApp* CreateEngineApp()
{
	return new Genesis::EditorApp();
}