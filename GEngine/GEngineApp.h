#pragma once
#include "GEngineWindow.h"
#include "GEngineContext.h"
#include "GEngineTimer.h"

int main(int argc, char** argv);

class GEngineApp
{
public:
	GEngineApp(int32_t width, int32_t height, std::string title);
	~GEngineApp();

	int Run();

private:
	GEngineTimer* m_Timer;
};

// To be defined in CLIENT
GEngineApp* CreateGEngineApp();

