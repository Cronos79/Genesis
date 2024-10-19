#pragma once
#include "GEngineImGuiObject.h"
class GEngineUIMainMenu : public GEngineImGuiObject
{
public:
    GEngineUIMainMenu();

	void OnImGuiUpdate() override;

private:
	void MainMenuBar();
	bool OpenNewProjectWindow();

private:
	bool m_OpenNewProject = false;
};

