/******************************************************************************************
*	CronoGames Game Engine																  *
*	Copyright © 2024 CronoGames <http://www.cronogames.net>								  *
*																						  *
*	This file is part of CronoGames Game Engine.										  *
*																						  *
*	CronoGames Game Engine is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The CronoGames Game Engine is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The CronoGames Game Engine.  If not, see <http://www.gnu.org/licenses/>.   *
******************************************************************************************/
#include "GEngine/Core/GEnginePCH.h"
#include "GContext.h"
#include "GEngine/Win/Window.h"
#include <GEngine/Logger/Log.h>

namespace Genesis
{
	DxgiInfoManager& GContext::GetInfoManager()
	{
		if (infoManager == nullptr)
		{
			infoManager = new DxgiInfoManager;
		}
		return *infoManager;
	}
	Window* GContext::GetWindow() const
	{
		return m_Window;
	}

	Genesis::Graphics* GContext::GetGraphics() const
	{
		return m_Graphics;
	}

	Genesis::Graphics12* GContext::GetGraphics12() const
	{
		return m_Graphics12;
	}

	void GContext::Init(int32 width, int32 height, std::string title)
	{
		LOG_INFO("Context initialized.");
		m_Window = new Window(width, height, title.c_str());
		m_Window->SetTitle(title);
		m_Graphics12 = new Graphics12(GetHWnd(), width, height);
	}	

	void GContext::Shutdown()
	{
		m_Window->Shutdown();
		m_Graphics12->Shutdown();
		LOG_INFO("Context shutdown.");
		delete Get();
	}
}