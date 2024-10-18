#include "genginepch.h"
#include "GEngineContext.h"

GEngineContext::GEngineContext()
{

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

GEngineSceneManager* GEngineContext::GetSceneMng()
{
	return m_Context.m_SceneMng;
}

void GEngineContext::InitWindow(int32_t width, int32_t height, std::string title)
{
	m_Context.m_wnd = new GEngineWindow(width, height, title.c_str());
}

void GEngineContext::InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd)
{
	m_Context.m_Gfx = new GEngineD3D12(width, height, hInstance, hWnd);
}

void GEngineContext::InitSceneMng()
{
	m_Context.m_SceneMng = new GEngineSceneManager();
}
