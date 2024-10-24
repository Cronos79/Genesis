#pragma once
#include "GEngineWindow.h"
#include "GEngineD3D12.h"
#include "GEngineProjectMng.h"

struct GEngineVersion
{
	int32_t Major;
	int32_t Minor;
	int32_t Patch;

	std::string ToString()
	{
		return std::format("{}.{}.{}", Major, Minor, Patch);
	}
};

enum GEngineRendererType
{
	D3D12
};

struct GEngineData
{
	GEngineWindow* m_wnd;
	GEngineD3D12* m_Gfx;
	GEngineProjectMng* m_ProjectMng;
	GEngineVersion m_Version;
	bool m_IsRunning = true;	
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
	GEngineWindow* GetWindow();
	GEngineD3D12* GetGFX();
	GEngineProjectMng* GetProjectMng();
	GEngineVersion GetVersion();

	inline bool IsRunning() { return m_Data.m_IsRunning; }
	inline void StopRunning() { m_Data.m_IsRunning = false; }

	bool Init(GEngineRendererType rType, int32_t width, int32_t height, std::string title, bool fullScreen);
	void ShutDown();
private:
	// Windows stuff
	bool InitWindow(int32_t width, int32_t height, std::string title);

	// Gfx
	bool InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd);

	// Scene manager
	bool InitProjectMng();


private:
	GEngineData m_Data;	
	GEngineRendererType m_RendererType;
};