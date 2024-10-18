#pragma once
#include "GEngineImGuiObject.h"
class GEngineTestUI : public GEngineImGuiObject
{
public:
	GEngineTestUI();

	void OnAttach() override;


	void OnDetach() override;


	void OnImGuiUpdate() override;
	bool show_another_window = true;
};

