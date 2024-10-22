#include "genginepch.h"
#include "GEngineContext.h"

GEngineContext::GEngineContext()
{
	m_Data.m_Version.Major = 0;
	m_Data.m_Version.Minor = 1;
	m_Data.m_Version.Patch = 2;
}

GEngineWindow* GEngineContext::GetWindow()
{
	return m_Data.m_wnd;
}

GEngineD3D12* GEngineContext::GetGFX()
{
	return m_Data.m_Gfx;
}

GEngineProjectMng* GEngineContext::GetProjectMng()
{
	return m_Data.m_ProjectMng;
}

GEngineVersion GEngineContext::GetVersion()
{
	return m_Data.m_Version;
}

void GEngineContext::InitWindow(int32_t width, int32_t height, std::string title)
{
	m_Data.m_wnd = new GEngineWindow(width, height, title.c_str());
}

void GEngineContext::InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd)
{
	m_Data.m_Gfx = new GEngineD3D12(width, height, hInstance, hWnd);
}

void GEngineContext::InitProjectMng()
{
	m_Data.m_ProjectMng = new GEngineProjectMng();
}