#include "genginepch.h"
#include "GEngineContext.h"

GEngineContext::GEngineContext()
{
	m_Version.Major = 0;
	m_Version.Minor = 1;
	m_Version.Patch = 0;
	std::string version = m_Version.ToString();
}

GEngineData GEngineContext::GetContext()
{
	return m_Context;
}

GEngineWindow* GEngineContext::GetWindow()
{
	return m_Context.m_wnd;
}

GEngineD3D12* GEngineContext::GetGFX()
{
	return m_Context.m_Gfx;
}

GEngineProjectMng* GEngineContext::GetProjectMng()
{
	return m_Context.m_ProjectMng;
}

GEngineVersion GEngineContext::GetVersion()
{
	return m_Version;
}

void GEngineContext::InitWindow(int32_t width, int32_t height, std::string title)
{
	m_Context.m_wnd = new GEngineWindow(width, height, title.c_str());
}

void GEngineContext::InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd)
{
	m_Context.m_Gfx = new GEngineD3D12(width, height, hInstance, hWnd);
}

void GEngineContext::InitProjectMng()
{
	m_Context.m_ProjectMng = new GEngineProjectMng();
}