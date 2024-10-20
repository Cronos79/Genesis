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
	bool OpenProjectWindow();

private:
	bool m_OpenNewProject = false;
	bool m_OpenProject = false;
};

