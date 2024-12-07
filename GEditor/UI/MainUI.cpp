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
#include "MainUI.h"
#include "imgui.h"
#include <imgui_impl_win32.h>
#include "GEngine/Core/GContext.h"
#include "GEngine/Win/Window.h"
#include "../Project/ProjectMng.h"

namespace Genesis
{

	void MainUI::Draw(float deltaTime)
	{
		if (m_ShowMainUI)
		{
			DrawMainUI(deltaTime);
		}
		if (m_ShowExtraTools)
		{
			DrawExtraTools(deltaTime);
		}
	}

	void MainUI::DrawMainUI(float deltaTime)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New"))
				{
				}
				if (ImGui::MenuItem("Open"))
				{
				}
				if (ImGui::MenuItem("Exit"))
				{
					GContext::Get().SetRunning(false);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		std::string projectName = ProjectMng::Get().GetCurrentProject().GetProjectName();
		std::string sceneName = ProjectMng::Get().GetCurrentProject().GetCurrentScene().GetSceneName();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Begin("RightPanel");
		ImGui::Text(projectName.c_str());
		ImGui::Text(sceneName.c_str());
		ImGui::End();
	}

	void MainUI::DrawExtraTools(float deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Begin("FPS");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();

		ImGui::Begin("Mouse");
		ImGui::Text("Mouse X %.1f Mouse Y %.1f", (float)GContext::Get().GetWindow()->mouse.GetPosX(), (float)GContext::Get().GetWindow()->mouse.GetPosY());
		ImGui::End();

		ImGui::Begin("Camera");
		ImGui::Text("Camera X %.1f Camera Y %.1f Camera Z %.1f", GContext::Get().GetGraphics()->GetDX11()->m_Camera.GetPositionFloat3().x, GContext::Get().GetGraphics()->GetDX11()->m_Camera.GetPositionFloat3().y, GContext::Get().GetGraphics()->GetDX11()->m_Camera.GetPositionFloat3().z);
		// Set camera position
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		ImGui::SliderFloat("Speed", &m_CameraSpeed, 0.0f, 5.0f);
		ImGui::SliderFloat("Camera X", &x, -1.0f, 1.0f);
		ImGui::SliderFloat("Camera Y", &y, -1.0f, 1.0f);
		ImGui::SliderFloat("Camera Z", &z, -1.0f, 1.0f);
		GContext::Get().GetGraphics()->GetDX11()->m_Camera.AdjustPosition(x * m_CameraSpeed * deltaTime, y * m_CameraSpeed * deltaTime, z * m_CameraSpeed * deltaTime);
		ImGui::End();

		ImGui::Begin("Alpha");
		ImGui::SliderFloat("Alpha", &GContext::Get().GetGraphics()->GetDX11()->m_Alpha, 0.0f, 1.0f);
		ImGui::End();
	}
}