#pragma once
#include "GEngineWindow.h"


struct GEngineData
{
	GEngineWindow* wnd;
};

class GEngineContext
{
public:
	GEngineContext()
	{

	}

	void InitWindow(int32_t width, int32_t height, std::string title)
	{
		context.wnd = new GEngineWindow(width, height, title.c_str());
	}

	GEngineData context;
};