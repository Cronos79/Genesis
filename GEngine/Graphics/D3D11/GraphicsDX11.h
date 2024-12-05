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
#include "GEngine/Core/CommonHeaders.h"
#include "GEngine/Graphics/D3D11/Shaders.h"

namespace Genesis
{
	class GraphicsDX11
	{
	public:
		GraphicsDX11();
		~GraphicsDX11();

		bool Initialize();
		void Shutdown();

		void BeginFrame(float deltaTime);
		void EndFrame(float deltaTime);

		inline ID3D11Device* GetDevice() const noexcept
		{
			return m_pDevice.Get();
		}

		inline ID3D11DeviceContext* GetDeviceContext() const noexcept
		{
			return m_pDeviceContext.Get();
		}

		inline IDXGISwapChain* GetSwapChain() const noexcept
		{
			return m_pSwapChain.Get();
		}

		inline ID3D11RenderTargetView* GetRenderTargetView() const noexcept
		{
			return m_pRenderTargetView.Get();
		}

	private:
		bool InitializeDirectX();
		bool InitializeShaders();
		bool InitializeScene();
		void ClearBuffer(float red, float green, float blue) noexcept;

	private:
		DxgiInfoManager* infoManager;
		ComPtr<ID3D11Device> m_pDevice;
		ComPtr<ID3D11DeviceContext> m_pDeviceContext;
		ComPtr<IDXGISwapChain> m_pSwapChain;
		ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;		
		ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
		VertexShader m_VertexShader;
		PixelShader m_PixelShader;

		ComPtr<ID3D11Buffer> m_pVertexBuffer;

		ComPtr<ID3D11RasterizerState> m_pRasterizerState;
	};
}
