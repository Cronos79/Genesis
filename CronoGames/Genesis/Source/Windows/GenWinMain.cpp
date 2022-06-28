/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "GenWindow.h"
#include "Utilitys/GenException.h"
#include "Game/Genesis.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR cmdLine, int)
{
	try
	{
		return Genesis{}.Start();
	}
	catch (const GenException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}