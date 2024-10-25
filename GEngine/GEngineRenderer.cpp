#include "genginepch.h"
#include "GEngineRenderer.h"
#include "GEngineContext.h"


GEngineRenderer::GEngineRenderer()
{

}

GEngineD3D12* GEngineRenderer::GetGFX()
{
	if (!m_Gfx)
	{
		return nullptr;
	}
	return m_Gfx;	
}

bool GEngineRenderer::Init()
{
	bool result = false;
	result = InitGfx(GEngineContext::GetInstance().GetWindow()->GetWidth(), GEngineContext::GetInstance().GetWindow()->GetHeight(), GEngineContext::GetInstance().GetWindow()->GetHInst(), GEngineContext::GetInstance().GetWindow()->GetHWND());

	return result;
}

void GEngineRenderer::Shutdown()
{
	m_Gfx->Shutdown();
	delete m_Gfx;
	m_Gfx = nullptr;
}

bool GEngineRenderer::InitGfx(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd)
{
	bool result = false;
	m_Gfx = new GEngineD3D12(width, height, hInstance, hWnd);
	if (m_Gfx)
	{
		m_Gfx->InIt();
		result = true;
	}
	return result;
}