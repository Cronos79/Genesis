#pragma once
#include "GEngineImGuiObject.h"

class UIMainMenu : public GEngineImGuiObject
{
public:
	UIMainMenu();

	void OnImGuiUpdate() override;

private:
	void MainMenuBar();
	bool OpenNewProjectWindow();
	bool OpenProjectWindow();
	bool SaveProjectWindow();

private:
	bool m_OpenNewProject = false;
	bool m_OpenProject = false;
	bool m_SaveProject = false;
};

