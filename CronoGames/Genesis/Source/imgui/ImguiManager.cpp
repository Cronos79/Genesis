/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "ImguiManager.h"
#include "imgui/imgui.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}