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

struct GEngineData
{
	GEngineWindow* m_wnd;
	GEngineD3D12* m_Gfx;
	GEngineProjectMng* m_ProjectMng;
	GEngineVersion m_Version;
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

	inline bool IsRunning() { return m_IsRunning; }
	inline void StopRunning() { m_IsRunning = false; }

	// Windows stuff
	void InitWindow(int32_t width, int32_t height, std::string title);

	// Gfx
	void InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd);

	// Scene manager
	void InitProjectMng();


private:
	GEngineData m_Data;	
	bool m_IsRunning = true;
};