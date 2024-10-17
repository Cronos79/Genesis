#include "GEditorApp.h"

GEditorApp::GEditorApp(int32_t width, int32_t height, std::string title)
	: GEngineApp(width, height, title)
{

}

GEditorApp::~GEditorApp()
{

}

GEngineApp* CreateGEngineApp()
{
	return new GEditorApp(1920, 1080, "hi");
}
