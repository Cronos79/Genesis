#include "genginepch.h"
#include "GEngineApp.h"

GEngineApp::GEngineApp(int32_t width, int32_t height, std::string title)
{
	context.InitWindow(width, height, title);
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
}
