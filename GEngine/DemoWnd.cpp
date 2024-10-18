#include "genginepch.h"
#include "DemoWnd.h"
#include "imgui.h"

DemoWnd::DemoWnd()
	: GEngineImGuiObject("DemoWnd")
{

}

void DemoWnd::OnImGuiUpdate()
{
	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
}
