#include "UIPanels.h"
#include "imgui.h"

UIPanels::UIPanels()
	: GEngineImGuiObject("UIPanels")
{

}

void UIPanels::OnImGuiUpdate()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Begin("Left panel"); 				// Create a window called "Hello, world!" and append into it.
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();
}
