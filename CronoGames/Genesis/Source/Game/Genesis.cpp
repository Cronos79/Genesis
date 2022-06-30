/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "Genesis.h"
#include "Graphics/GDIPlusManager.h"
#include "imgui/imgui_impl_dx11.h"

GDIPlusManager gdipm;

Genesis::Genesis()
	:
	wnd(1920, 1080, "Genesis"),
	light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)wnd.GetWndDem().height / (float)wnd.GetWndDem().width, 0.5f, 40.0f));
}

int Genesis::Start()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = GenWindow::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		Update();
	}
}

void Genesis::Update()
{	
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	if (wnd.kbd.KeyIsPressed('R'))
	{
		wnd.Gfx().DisableImgui();
	}
	else
	{
		wnd.Gfx().EnableImgui();
	}
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	//nano.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	//nano.ShowWindow();
	DisplayFPS(dt);
	// present
	wnd.Gfx().EndFrame();
}

void Genesis::DisplayFPS(float dt)
{
	static float fpstime;
	fpstime += (dt * 1000);
	// FPS
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpstime > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter) + "\n";
		fpsCounter = 0;
		fpstime = 0;
	}
	//Create ImGui Test Window
	ImGui::Begin("App info");
	ImGui::Text(fpsString.c_str());
	ImGui::End();
}
