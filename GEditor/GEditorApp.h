#pragma once
#include "GEditorBase.h"
#include "WinEntry.cpp"

class GEditorApp : public GEngineApp
{
public:
	GEditorApp(int32_t width, int32_t height, std::string title);
	~GEditorApp();
};

//GEngineApp* CreateGEngineApp(int32_t width, int32_t height, std::string title)
//{
//	return new GEditorApp(width, height, title);
//}