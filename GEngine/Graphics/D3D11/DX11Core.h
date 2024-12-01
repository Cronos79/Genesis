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
#include "GEngine/Graphics/GraphicsIncludes.h"
#include <GEngine/Core/GenException.h>

namespace Genesis
{
	class DX11Core
	{
	public:
		DX11Core();
		DX11Core(const DX11Core&) = delete;
		DX11Core& operator=(const DX11Core&) = delete;
		~DX11Core();

		inline ID3D11Device* GetDevice() const noexcept
		{
			return m_device.Get();
		}

		void Init();
		void Shutdown();
		void BeginFrame(float deltaTime);
		void EndFrame(float deltaTime);
		void DrawTriangle(float angle, float x, float y);
		void DrawCube(float angle, float x, float y, float z);
	private:		
		void ClearBuffer(float red, float green, float blue) noexcept;
	private:
		ComPtr<ID3D11Device> m_device;
		ComPtr<ID3D11DeviceContext> m_deviceContext;
		ComPtr<IDXGISwapChain> m_swapChain;		
		ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		ComPtr<ID3D11Buffer> m_vertexBuffer;

		DxgiInfoManager* infoManager;
	};
}


