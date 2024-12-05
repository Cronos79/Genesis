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
#include "GEngine/Graphics/GraphicsIncludes.h"
#include "D3D11/GraphicsDX11.h"

enum class GraphicsAPI
{
	OpenGL,
	DirectX11,
	DirectX12,
	Vulkan
};

namespace Genesis
{
	class GenGraphics
	{
	public:
		GenGraphics(GraphicsAPI api);
		~GenGraphics();
		void Init();
		void Shutdown();
		void BeginFrame(float deltaTime);
		void EndFrame(float deltaTime);
		void DrawIndexed(UINT count) noexcept(!IS_DEBUG);

		inline GraphicsDX11* GetDX11() const noexcept
		{
			return m_DX11;
		}

		inline DxgiInfoManager* GetInfoManager(GenGraphics* gfx) const noexcept
		{
			return infoManager;
		}

	private:
		GraphicsAPI m_API;
		GraphicsDX11* m_DX11;
		DxgiInfoManager* infoManager;
	};
} // namespace Genesis
