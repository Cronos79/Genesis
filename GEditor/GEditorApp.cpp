#include "GEditorApp.h"
#include "UIManager.h"


GEditorApp::GEditorApp(int32_t width, int32_t height, std::string title, bool fullScreen)
	: GEngineApp(width, height, title, fullScreen)
{
	UIManager::SetUIOverlay();	
}

GEditorApp::~GEditorApp()
{

}

GEngineApp* CreateGEngineApp()
{
	HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info{};
	info.cbSize = sizeof(info);
	if (GetMonitorInfoW(monitor, &info))
	{
		RECT wr;
		wr.left = info.rcMonitor.left;
		wr.right = info.rcMonitor.right - info.rcMonitor.left;
		wr.top = info.rcMonitor.top;
		wr.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

		std::string title = std::format("Genesis Editor version: {}", GEngineContext::GetInstance().GetVersion().ToString());
		return new GEditorApp(wr.right - wr.left, wr.bottom - wr.top, title, false);
	}

	std::string title = std::format("Genesis Editor version: {}", GEngineContext::GetInstance().GetVersion().ToString());
	return new GEditorApp(1920, 1080, title, false);
}
