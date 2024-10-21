#include "GEditorApp.h"
#include "UIManager.h"


GEditorApp::GEditorApp(int32_t width, int32_t height, std::string title, bool fullScreen)
	: GEngineApp(width, height, title, fullScreen)
{
	//UIManager::SetUIOverlay();
}

GEditorApp::~GEditorApp()
{

}

GEngineApp* CreateGEngineApp()
{
	std::string title = std::format("Genesis Editor version: {}", GEngineContext::GetInstance().GetVersion().ToString());
	return new GEditorApp(1920, 1080, title, true);
}
