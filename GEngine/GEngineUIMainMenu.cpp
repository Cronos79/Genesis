#include "genginepch.h"
#include "GEngineUIMainMenu.h"
#include "imgui.h"
#include <Windows.h>

GEngineUIMainMenu::GEngineUIMainMenu()
	: GEngineImGuiObject("MainMenu") { }

void GEngineUIMainMenu::OnImGuiUpdate()
{
	MainMenuBar();
	if (m_OpenNewProject)
	{
		OpenNewProjectWindow();
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
	if (ImGui::Begin("NewProject", &m_OpenNewProject))
	{
		ImGui::End();
	}	
	return true;
}
