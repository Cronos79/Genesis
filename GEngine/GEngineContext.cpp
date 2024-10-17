#include "genginepch.h"
#include "GEngineContext.h"

GEngineContext::GEngineContext()
{

}

GEngineData GEngineContext::GetContext()
{
	return m_Context;
}

void GEngineContext::InitWindow(int32_t width, int32_t height, std::string title)
{
	m_Context.m_wnd = new GEngineWindow(width, height, title.c_str());
}