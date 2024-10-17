#pragma once
#include "GEngineWindow.h"


struct GEngineData
{
	GEngineWindow* m_wnd;
};

class GEngineContext
{
public:
	GEngineContext();

	// Core stuff
	GEngineData GetContext();

	// Windows stuff
	void InitWindow(int32_t width, int32_t height, std::string title);


private:
	GEngineData m_Context;
};