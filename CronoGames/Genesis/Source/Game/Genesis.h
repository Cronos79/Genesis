/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "Windows/GenWindow.h"
#include "Utilitys/GenTimer.h"
#include <vector>
#include "Graphics/Drawable.h"
#include "imgui/ImguiManager.h"
class Genesis
{
public:
	Genesis();
	// master frame / message loop
	int Start();
private:
	void Update();
	void DisplayFPS(float dt);
private:
	ImguiManager imgui;
	GenWindow wnd;
	GenTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
};

