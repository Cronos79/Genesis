/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "Genesis.h"
#include "Graphics/GDIPlusManager.h"
#include "imgui/imgui_impl_dx11.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

GDIPlusManager gdipm;

bool DoTheImportThing(const std::string& pFile) {
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (scene == nullptr) {
		//DoTheErrorLogging(importer.GetErrorString());
		return false;
	}

	// Now we can access the file's contents.
	//DoTheSceneProcessing(scene);

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}

Genesis::Genesis()
	:
	wnd(1920, 1080, "Genesis"),
	light(wnd.Gfx())
{
	DoTheImportThing("Cube.fbx");
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)wnd.GetWndDem().height / (float)wnd.GetWndDem().width, 0.5f, 400.0f)); //#NOTE: This sets the view distance of objects
	wnd.mouse.EnableRaw();
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

	if (wnd.kbd.KeyIsPressed('W'))
	{
		cam.Translate({ 0.0f,0.0f,dt });
	}
	if (wnd.kbd.KeyIsPressed('A'))
	{
		cam.Translate({ -dt,0.0f,0.0f });
	}
	if (wnd.kbd.KeyIsPressed('S'))
	{
		cam.Translate({ 0.0f,0.0f,-dt });
	}
	if (wnd.kbd.KeyIsPressed('D'))
	{
		cam.Translate({ dt,0.0f,0.0f });
	}
	if (wnd.kbd.KeyIsPressed('R'))
	{
		cam.Translate({ 0.0f,dt,0.0f });
	}
	if (wnd.kbd.KeyIsPressed('F'))
	{
		cam.Translate({ 0.0f,-dt,0.0f });
	}


	while (const auto delta = wnd.mouse.ReadRawDelta())
	{

		cam.Rotate((float)delta->x, (float)delta->y);

	}


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
	//light2.Bind(wnd.Gfx(), cam.GetMatrix());

	cube.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	//light2.Draw(wnd.Gfx());

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	//light2.SpawnControlWindow();
	cube.ShowWindow();
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
