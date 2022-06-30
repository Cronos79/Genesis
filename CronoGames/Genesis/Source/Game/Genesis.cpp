/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "Genesis.h"
#include "Graphics/Melon.h"
#include "Graphics/Pyramid.h"
#include "Graphics/Box.h"
#include "Graphics/Sheet.h"
#include "Graphics/SkinnedBox.h"
#include <memory>
#include <algorithm>
#include "Utilitys/GenMath.h"
#include "Graphics/Surface.h"
#include "Graphics/GDIPlusManager.h"
#include "Graphics/Drawable.h"
#include "imgui/imgui_impl_dx11.h"

GDIPlusManager gdipm;

Genesis::Genesis()
	:
	wnd(1920, 1080, "Genesis")
{
	class Factory
	{
	public:
		Factory(GenGraphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			case 3:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 4:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		GenGraphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 };
	};

	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory{ wnd.Gfx() });

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
	const auto dt = timer.Mark();
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

	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}
	cam.SpawnControlWindow();
	DisplayFPS(dt);
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
