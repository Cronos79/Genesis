// Copyright © 2024 CronoGames
// Created by: Matt Brock
//#include <Windows.h>
#include "GEngine/Application/EngineApp.h"
#include <GEngine/src/win/IWindow.h>
#include <GEngine/src/log/Log.h>
#include <GEngine/src/ioc/Container.h>
#include <GEngine/src/log/SeverityLevelPolicy.h>
#include <GEngine/src/win/Boot.h>

extern Genesis::EngineApp* CreateEngineApp();

using namespace chil;
using namespace std::string_literals;

void Boot()
{
	log::Boot();
	ioc::Get().Register<log::ISeverityLevelPolicy>([] {
		return std::make_shared<log::SeverityLevelPolicy>(log::Level::Info);
		});

	win::Boot();
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
		try {
		Boot();
		auto pWindow = ioc::Get().Resolve<win::IWindow>();
		Genesis::EngineApp* app = CreateEngineApp();
		return app->Run(*pWindow);
	}
	catch (const std::exception& e) {
		chilog.error(utl::ToWide(e.what())).no_line().no_trace();
		MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR | MB_SETFOREGROUND);
	}
	return -1;	
}