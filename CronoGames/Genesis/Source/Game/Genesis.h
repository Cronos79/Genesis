/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "Windows/GenWindow.h"
#include "Utilitys/GenTimer.h"
#include "imgui/ImguiManager.h"
#include "Graphics/GenCamera.h"
#include "Graphics/PointLight.h"
#include "Graphics/Mesh.h"
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
	GenCamera cam;
	ImguiManager imgui;
	GenWindow wnd;
	GenTimer timer;
	float speed_factor = 1.0f;
	PointLight light;
	//PointLight light2;
	Model cube{ wnd.Gfx(),"Cube.fbx" };
};

