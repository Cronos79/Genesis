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
	GEngineContext::GetInstance().Init(GEngineRendererType::D3D12, width, height, title, fullScreen);	
}

GEngineApp::~GEngineApp()
{
}

int GEngineApp::Run()
{
	while (GEngineContext::GetInstance().IsRunning())
	{		
		if (const auto ecode = GEngineWindow::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			ShutDown();
			return *ecode;
		}
		//if ((GEngineContext::GetInstance().GetRenderer() != nullptr) && (GEngineContext::GetInstance().GetRenderer()->GetGFX() != nullptr))
		if (GEngineContext::GetInstance().GetRenderer() != nullptr)
		{
			float deltaTime = m_Timer->Mark();

			GEngineContext::GetInstance().GetRenderer()->GetGFX()->BeginFrame(deltaTime);
			
			for (auto* gobjects : GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_SceneManager->GetCurrentScene()->GetGameObjects())
			{
				if (gobjects->IsFirstRun())
				{
					gobjects->OnStart();
				}				
				gobjects->OnHandleInput(deltaTime);
				gobjects->OnUpdate(deltaTime);
			}

			for (auto* gobjects : GEngineContext::GetInstance().GetProjectMng()->GetCurrentProject()->m_SceneManager->GetCurrentScene()->GetGameObjects())
			{
				if (dynamic_cast<GEngineImGuiObject*>(gobjects))
				{
					gobjects->OnImGuiUpdate();
				}
			}

			GEngineContext::GetInstance().GetRenderer()->GetGFX()->EndFrame(deltaTime);
		}
	}
	return 0;
}

void GEngineApp::ShutDown()
{
	GEngineContext::GetInstance().ShutDown();
}
