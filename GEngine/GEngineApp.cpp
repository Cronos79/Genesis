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
				std::exit(0);
				//GEngineContext::GetInstance().StopRunning(); // #TODO: Cant stop or breakpoint happens
				//DestroyWindow(GEngineContext::GetInstance().GetWindow()->GetHWND());
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
	return 0;
}