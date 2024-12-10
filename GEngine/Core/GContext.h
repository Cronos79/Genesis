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
#pragma once
#include "GEngine/Win/WinInclude.h"
#include "GEngine/Core/GEnginePCH.h"
#include <GEngine/Graphics/Graphics.h>
#include <GEngine/Graphics/Graphics12.h>
#include "GenException.h"

namespace Genesis
{
	class Window;

	class GContext
	{
	public:
		inline void SetWidth(int32 width)
		{
			m_Width = width;
		}
		inline void SetHeight(int32 height)
		{
			m_Height = height;
		}
		inline void SetHWnd(HWND hWnd)
		{
			m_HWnd = hWnd;
		}
		inline void SetRunning(bool running)
		{
			m_Running = running;
		}
		inline int32 GetWidth() const
		{
			return m_Width;
		}
		inline int32 GetHeight() const
		{
			return m_Height;
		}
		inline HWND GetHWnd() const
		{
			return m_HWnd;
		}
		inline bool IsRunning() const
		{
			return m_Running;
		}	
		DxgiInfoManager& GetInfoManager();
		Window* GetWindow() const;
		Graphics* GetGraphics() const;
		Graphics12* GetGraphics12() const;

		void Init(int32 width, int32 height, std::string title);
		void Shutdown();
	private:
		int32 m_Width;
		int32 m_Height;
		HWND m_HWnd;
		Window* m_Window;
		Graphics* m_Graphics;
		Graphics12* m_Graphics12;
		bool m_Running = false;

		DxgiInfoManager* infoManager;

		// Singleton
	public:
		inline static GContext* Get()
		{
			static GContext* instance;
			if (!instance)
			{
				instance = new GContext();
			}
			return instance;
		}
		// Delete copy constructor and assignment operator
		GContext(const GContext&) = delete;
		GContext& operator=(const GContext&) = delete;
	private:
		GContext() = default;
		~GContext() = default;
	};
}
