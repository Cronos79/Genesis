#pragma once
#include "GEngineWindow.h"
#include "GEngineD3D12.h"


struct GEngineData
{
	GEngineWindow* m_wnd;
	GEngineD3D12* m_Gfx;
};

class GEngineContext
{
public:
	GEngineContext();
	GEngineContext(const GEngineContext&) = delete;
	GEngineContext& operator=(const GEngineContext&) = delete;

	static GEngineContext& GetInstance() {
		static GEngineContext instance; // Guaranteed to be destroyed and instantiated on first use
		return instance;
	}

	// Core stuff
	GEngineData GetContext();
	GEngineWindow* GetWindow();
	GEngineD3D12* GetGFX();

	// Windows stuff
	void InitWindow(int32_t width, int32_t height, std::string title);

	// Gfx
	void InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd);


private:
	GEngineData m_Context;
};