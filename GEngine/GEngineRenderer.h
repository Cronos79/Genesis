#pragma once
#include "genginepch.h"
#include "WinInclude.h"
#include "GEngineD3D12.h"

class GEngineRenderer
{
public:
	GEngineRenderer();

	GEngineD3D12* GetGFX();

	bool Init();
	void Shutdown();

private:
	bool InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd);

private:
	GEngineD3D12* m_Gfx;
};

