#include "genginepch.h"
#include "GEngineApp.h"
#include "GEngineLog.h"

GEngineApp::GEngineApp(int32_t width, int32_t height, std::string title)
{
	m_Timer = new GEngineTimer();
	GEngineLog::Init();
	BB_TRACE("Genesis starting");
	m_Context.InitWindow(width, height, title);

}

GEngineApp::~GEngineApp()
{

}

int GEngineApp::Run()
{
	while (true)
	{		
		if (const auto ecode = GEngineWindow::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
	}

	float deltaTime = m_Timer->Mark();
}
