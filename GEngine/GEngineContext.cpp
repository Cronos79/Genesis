#include "genginepch.h"
#include "GEngineContext.h"
#include "GEngineDXDebugLayer.h"

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

bool GEngineContext::Init(GEngineRendererType rType, int32_t width, int32_t height, std::string title, bool fullScreen)
{
	m_RendererType = rType;
	switch (m_RendererType)
	{
	case D3D12: 
	{
		bool result = false;
		result = InitWindow(width, height, title);
		if (result)
		{
			result = InitGfx(width, height, m_Data.m_wnd->GetHInst(), m_Data.m_wnd->GetHWND());
			if (!result) { return false; }

			result = InitProjectMng();
			if (!result) { return false; }

			if (fullScreen)
			{
				m_Data.m_wnd->SetFullScreen(fullScreen);
			}

			return result;
		}
	}
	default:
		return false;
	}
	return false;
}

void GEngineContext::ShutDown()
{
	switch (m_RendererType)
	{
	case D3D12:
	{
		m_Data.m_ProjectMng->ShutDown();
		delete m_Data.m_ProjectMng;
		m_Data.m_ProjectMng = nullptr;

		m_Data.m_Gfx->Shutdown();
		delete m_Data.m_Gfx;
		m_Data.m_Gfx = nullptr;

		m_Data.m_wnd->ShutDown();
		delete m_Data.m_wnd;
		m_Data.m_wnd = nullptr;

		GEngineDXDebugLayer::Get().Shutdown();
	}
	break;
	default:
		break;
	}
}

bool GEngineContext::InitWindow(int32_t width, int32_t height, std::string title)
{
	m_Data.m_wnd = new GEngineWindow(width, height, title.c_str());
	if (m_Data.m_wnd)
	{
		m_Data.m_wnd->Init();
		return true;
	}
	return false;
}

bool GEngineContext::InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd)
{
	m_Data.m_Gfx = new GEngineD3D12(width, height, hInstance, hWnd);
	if (m_Data.m_Gfx)
	{
		m_Data.m_Gfx->InIt();
		return true;
	}
	return false;
}

bool GEngineContext::InitProjectMng()
{
	m_Data.m_ProjectMng = new GEngineProjectMng();	
	if (m_Data.m_ProjectMng)
	{
		m_Data.m_ProjectMng->Init();
		return true;
	}
	return false;
}