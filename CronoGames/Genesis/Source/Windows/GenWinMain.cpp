/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "GenWindow.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR cmdLine, int)
{
	GenWindow wnd(1920, 1080, "Genesis");

	MSG msg;
	bool running = true;
	while (running)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_QUIT:
			{
				running = false;
				break;
			}
			default:
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// wParam here is the value passed to PostQuitMessage
	return msg.wParam;
}