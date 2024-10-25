#include "genginepch.h"
#include "GEngineContext.h"
#include "GEngineDXDebugLayer.h"

GEngineContext::GEngineContext()
{

}

GEngineWindow* GEngineContext::GetWindow()
{
	return m_Data.m_wnd;
}


GEngineRenderer* GEngineContext::GetRenderer()
{
	return m_Data.m_Renderer;
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
			result = InitRenderer();
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

		m_Data.m_Renderer->Shutdown();
		delete m_Data.m_Renderer;
		m_Data.m_Renderer = nullptr;

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

bool GEngineContext::InitRenderer()
{
	m_Data.m_Renderer = new GEngineRenderer();
	if (m_Data.m_Renderer)
	{
		m_Data.m_Renderer->Init();
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