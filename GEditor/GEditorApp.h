#pragma once
#include "GEngineAPI.h"
#include "WinEntry.cpp"

class GEditorApp : public GEngineApp
{
public:
	GEditorApp(int32_t width, int32_t height, std::string title, bool fullScreen);
	~GEditorApp();	
};