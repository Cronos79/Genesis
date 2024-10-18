#include "genginepch.h"
#include "GEngineApp.h"
#include "GEngineLog.h"
#include "GEngineImGuiObject.h"

GEngineApp::GEngineApp(int32_t width, int32_t height, std::string title)
{
	m_Timer = new GEngineTimer();
	GEngineLog::Init();
	BB_TRACE("Genesis starting");
	GEngineContext::GetInstance().InitWindow(width, height, title);
	GEngineContext::GetInstance().InitGfx(width, height, GEngineContext::GetInstance().GetWindow()->GetHInst(), GEngineContext::GetInstance().GetWindow()->GetHWND());
}

GEngineApp::~GEngineApp()
{

}

int GEngineApp::Run()
{
	GEngineImGuiObject* gobject = new GEngineImGuiObject("imgui");
	gobject->OnAttach();
	while (true)
	{		
		if (const auto ecode = GEngineWindow::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		float deltaTime = m_Timer->Mark();

		
		GEngineContext::GetInstance().GetGFX()->BeginRender(deltaTime);

		gobject->Begin();
		gobject->OnImGuiRender();
		gobject->End();

		GEngineContext::GetInstance().GetGFX()->EndRender(deltaTime);
				
	}
	gobject->OnDetach();

}
