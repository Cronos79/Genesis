#include "genginepch.h"
#include "GEngineUIMainMenu.h"
#include "imgui.h"
//#include <Windows.h>
#include "GEngineProjectMng.h"
#include "GEngineLog.h"
#include "GEngineContext.h"

GEngineUIMainMenu::GEngineUIMainMenu()
	: GEngineImGuiObject("MainMenu") { }

void GEngineUIMainMenu::OnImGuiUpdate()
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
}

void GEngineUIMainMenu::MainMenuBar()
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

		}
		if (ImGui::MenuItem("Exit"))
		{
			// PostQuitMessage(0); // #TODO: Breaks imgui on exit
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
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

bool GEngineUIMainMenu::OpenNewProjectWindow()
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
			m_OpenNewProject = false;
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

bool GEngineUIMainMenu::OpenProjectWindow()
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
