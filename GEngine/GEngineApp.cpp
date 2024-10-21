#include "genginepch.h"
#include "GEngineApp.h"
#include "GEngineLog.h"
#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "GEngineContext.h"

GEngineApp::GEngineApp(int32_t width, int32_t height, std::string title, bool fullScreen)
{
	m_Timer = new GEngineTimer();
	GEngineLog::Init();
	BB_TRACE("Genesis starting");
	GEngineContext::GetInstance().InitWindow(width, height, title);
	GEngineContext::GetInstance().InitGfx(width, height, GEngineContext::GetInstance().GetWindow()->GetHInst(), GEngineContext::GetInstance().GetWindow()->GetHWND());
	GEngineContext::GetInstance().InitProjectMng();

	if (fullScreen)
	{
		GEngineContext::GetInstance().GetWindow()->SetFullScreen(fullScreen);
	}

	InitImGui();	
}

GEngineApp::~GEngineApp()
{
	m_gobject->OnDetach();
}

int GEngineApp::Run()
{	
	while (GEngineContext::GetInstance().IsRunning())
	{		
		if (const auto ecode = GEngineWindow::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			//ShutdownImGui();				
			return *ecode;
		}
		if (GEngineContext::GetInstance().IsRunning())
		{
			float deltaTime = m_Timer->Mark();

			/****************************************************************/
			if (GEngineContext::GetInstance().GetWindow()->m_Kbd.KeyIsPressed(VK_F11))
			{
				GEngineContext::GetInstance().GetWindow()->SetFullScreen(!GEngineContext::GetInstance().GetWindow()->GetIsFullScreen());
			}
			if (GEngineContext::GetInstance().GetWindow()->m_Kbd.KeyIsPressed(VK_F12))
			{
				//GEngineContext::GetInstance().StopRunning(); // #TODO: Cant stop or breakpoint happens
			}
			/****************************************************************/

			GEngineContext::GetInstance().GetGFX()->BeginFrame(deltaTime);

			for (auto* gobjects : GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_SceneManager->GetCurrentScene()->GetGameObjects().GetGameObjects())
			{
				gobjects->OnUpdate(deltaTime);
			}

			for (auto* gobjects : GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_SceneManager->GetCurrentScene()->GetGameObjects().GetGameObjects())
			{
				if (dynamic_cast<GEngineImGuiObject*>(gobjects))
				{
					gobjects->OnImGuiUpdate();
				}
			}

			GEngineContext::GetInstance().GetGFX()->EndFrame(deltaTime);
		}
	}
}

 void GEngineApp::InitImGui()
 {
 	// Setup Dear ImGui context
 	IMGUI_CHECKVERSION();
 	ImGui::CreateContext();
 	ImGuiIO& io = ImGui::GetIO(); (void)io;
 	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
 	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
 	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
 	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
 	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
 	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
 
 	//float fontSize = 18.0f;// *2.0f;
 	//io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
 	//io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);
 
 	// Setup Dear ImGui style
 	ImGui::StyleColorsDark();
 	//ImGui::StyleColorsClassic();
 
 	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
 	ImGuiStyle& style = ImGui::GetStyle();
 	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
 	{
 		style.WindowRounding = 0.0f;
 		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
 	}
 
 	SetDarkThemeColors();
 
 	// Setup Platform/Renderer backends
 	ImGui_ImplWin32_Init(GEngineContext::GetInstance().GetWindow()->GetHWND());
 	/*ImGui_ImplDX12_Init(*GEngineContext::GetInstance().GetGFX()->GetDevice().GetAddressOf(), NUM_FRAMES_IN_FLIGHT,
 		DXGI_FORMAT_R8G8B8A8_UNORM, GEngineContext::GetInstance().GetGFX()->GetSrvDescHeap(),
 		GEngineContext::GetInstance().GetGFX()->GetSrvDescHeap()->GetCPUDescriptorHandleForHeapStart(),
 		GEngineContext::GetInstance().GetGFX()->GetSrvDescHeap()->GetGPUDescriptorHandleForHeapStart());*/
 }

void GEngineApp::ShutdownImGui()
{
	//ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GEngineApp::SetDarkThemeColors()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}
