#include "UIMainMenu.h"

#include "imgui.h"
#include "GEngineProjectMng.h"
#include "GEngineLog.h"
#include "GEngineContext.h"
#include "spdlog/fmt/bundled/format.h"
#include "UIManager.h"
#include "GEngineProjectSettings.h"
#include "TestTriangle.h"

UIMainMenu::UIMainMenu()
	: GEngineImGuiObject("MainMenu") { }

void UIMainMenu::OnImGuiUpdate()
{
	MainMenuBar();
	if (m_OpenNewProject)
	{
		OpenNewProjectWindow();
	}
	if (m_OpenProject)
	{
		OpenProjectWindow();
	}
	if (m_SaveProject)
	{
		SaveProjectWindow();
	}
}

void UIMainMenu::MainMenuBar()
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New Project"))
		{
			m_OpenNewProject = true;
		}
		if (ImGui::MenuItem("Open Project"))
		{
			m_OpenProject = true;
		}
		if (ImGui::MenuItem("Save"))
		{
			m_SaveProject = true;
		}
		if (ImGui::MenuItem("Exit"))
		{
			std::exit(0);
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Copy"))
		{

		}
		if (ImGui::MenuItem("Paste"))
		{

		}
		if (ImGui::MenuItem("Cut"))
		{

		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("About"))
	{
		ImGui::Text("Genesis editor");
		ImGui::Text(std::format("GEngine {}", GEngineContext::GetInstance().GetVersion().ToString()).c_str());
		ImGui::EndMenu();
	}
	ImGui::Text(std::format(" - Project: {}", GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_Data.ProjectName).c_str());
	ImGui::EndMainMenuBar();
}

bool UIMainMenu::OpenNewProjectWindow()
{
	ImGui::OpenPopup("NewProject");
	if (ImGui::BeginPopup("NewProject"))
	{
		static std::string projectName;
		static char projectNameBuffer[1028];
		strcpy_s(projectNameBuffer, sizeof(projectNameBuffer), projectName.c_str());
		if (ImGui::InputText("Project Name", projectNameBuffer, sizeof(projectNameBuffer)))
		{
			projectName = projectNameBuffer;
		}

		static std::string projectDesc;
		static char projectDescBuffer[1028];
		strcpy_s(projectDescBuffer, sizeof(projectDescBuffer), projectDesc.c_str());
		if (ImGui::InputText("Project Desc", projectDescBuffer, sizeof(projectDescBuffer)))
		{
			projectDesc = projectDescBuffer;
		}
		if (ImGui::Button("Create Project"))
		{
			ProjectData data = {};
			data.ProjectName = projectName;
			data.Description = projectDesc;
			data.EngineVersion = GEngineContext::GetInstance().GetVersion().ToString();

			GEngineProjectMng* mng = new GEngineProjectMng();
			mng->GESaveProject(data);
			bool wasFound = false;
			for (auto project : GEngineContext::GetInstance().GetProjectMng()->GetAllProjects())
			{
				if (project->m_Data.ProjectName == data.ProjectName)
				{
					GEngineContext::GetInstance().GetProjectMng()->SetCurrentProject(project);
					UIManager::SetUIOverlay();
					project->m_ProjectSettings->SaveSettings();
					wasFound = true;
				}				
			}
			if (!wasFound)
			{
				BB_INFO("Project creation error");
				ImGui::Text("Project creation error");
				m_OpenNewProject = true;
			}
			else
			{
				BB_INFO("Project Created!");
				m_OpenNewProject = false;
			}
			
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			m_OpenNewProject = false;
			
		}
		ImGui::EndPopup();
	}
	return true;
}

bool UIMainMenu::OpenProjectWindow()
{
	ImGui::OpenPopup("OpenProject");
	if (ImGui::BeginPopup("OpenProject"))
	{
		ImGui::Text("Click a project name to open it");
		for (auto project : GEngineContext::GetInstance().GetProjectMng()->GetAllProjects())
		{
			if (ImGui::Selectable(std::format("ProjectName: {} - EngineVersion: {}", project->m_Data.ProjectName, project->m_Data.EngineVersion).c_str()))
			{
				GEngineContext::GetInstance().GetProjectMng()->SetCurrentProject(project);
				GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_SceneManager->GetCurrentScene()->PushGameObject(new TestTriangle());
				UIManager::SetUIOverlay();
				m_OpenProject = false;
			}
		}

		if (ImGui::Button("Cancel"))
		{
			m_OpenProject = false;
		}
		ImGui::EndPopup();
	}
	return true;
}

bool UIMainMenu::SaveProjectWindow()
{
	ImGui::OpenPopup("SaveProject");
	if (ImGui::BeginPopup("SaveProject"))
	{
		ImGui::Text("Save a project");
		if (ImGui::Button("Cancel"))
		{
			m_SaveProject = false;
		}
		ImGui::EndPopup();
	}
	return true;
}