#include "genginepch.h"
#include "GEngineApp.h"
#include <Windows.h>
#include "stdint.h"
#include <exception>
#include "GEngineException.h"
#include "GEngineDXDebugLayer.h"
#include "GEngineLog.h"

extern GEngineApp* CreateGEngineApp();

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	int result = -1;
	try
	{
		auto app = CreateGEngineApp();
		result = app->Run();
		GEngineContext::GetInstance().GetGFX()->Shutdown();
		GEngineDXDebugLayer::Get().Shutdown();
		return result;
	}
	catch (const GEngineException& e)
	{
		BB_ERROR(e.what());
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		BB_ERROR(e.what());
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		BB_ERROR("Unknown Exception");
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return result;
}

#ifdef _DEBUG
int main(int argc, char** argv)
{
	return WinMain(GetModuleHandle(0), 0, ::GetCommandLine(), SW_SHOW);
}
#endif
