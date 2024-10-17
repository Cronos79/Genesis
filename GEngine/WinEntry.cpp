#include "genginepch.h"
#include "GEngineApp.h"
#include <Windows.h>
#include "stdint.h"
#include <exception>

extern GEngineApp* CreateGEngineApp();

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	auto app = CreateGEngineApp();
	try
	{
		app->Run();
	}
	catch (...)
	{
	}
	//try
	//{
	//	return App{ lpCmdLine }.Go();
	//}
	//catch (const ChiliException& e)
	//{
	//	MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	//}
	//catch (const std::exception& e)
	//{
	//	MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	//}
	//catch (...)
	//{
	//	MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	//}
	return -1;
}

int main(int argc, char** argv)
{
	return WinMain(GetModuleHandle(0), 0, ::GetCommandLine(), SW_SHOW);
}

