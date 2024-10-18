#pragma once
#include "GEngineWindow.h"
#include "GEngineContext.h"
#include "GEngineTimer.h"
#include "GEngineImGuiObject.h"

int main(int argc, char** argv);

class GEngineApp
{
public:
	GEngineApp(int32_t width, int32_t height, std::string title);
	~GEngineApp();

	int Run();

private:
	void InitImGui();
	void ShutdownImGui();
	void SetDarkThemeColors();

private:
	GEngineTimer* m_Timer;
	GEngineImGuiObject* m_gobject;
};

// To be defined in CLIENT
GEngineApp* CreateGEngineApp();

