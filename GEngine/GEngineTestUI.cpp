#include "genginepch.h"
#include "GEngineTestUI.h"
#include "imgui.h"

GEngineTestUI::GEngineTestUI()
	: GEngineImGuiObject("TestUI")
{

}

void GEngineTestUI::OnAttach()
{
	
}

void GEngineTestUI::OnDetach()
{
	
}

void GEngineTestUI::OnImGuiUpdate()
{
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}
