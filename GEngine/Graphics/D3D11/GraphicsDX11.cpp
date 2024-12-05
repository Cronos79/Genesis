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
#include "GraphicsDX11.h"
#include "AdapterReader.h"
#include "../Core/GenException.h"
#include "../Core/GContext.h"
#include <imgui_impl_dx11.h>

namespace Genesis
{
	GraphicsDX11::GraphicsDX11()
	{
	}
	GraphicsDX11::~GraphicsDX11()
	{
	}
	bool GraphicsDX11::Initialize()
	{
		return InitializeDirectX();
	}
	void GraphicsDX11::Shutdown()
	{
	}

	void GraphicsDX11::BeginFrame(float deltaTime)
	{
		if (!m_IsInitialized)
		{
			return;
		}
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void GraphicsDX11::EndFrame(float deltaTime)
	{
		if (!m_IsInitialized)
		{
			return;
		}
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		m_pSwapChain->Present(1u, 0u);
	}

	void GraphicsDX11::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
	{

	}

	bool GraphicsDX11::InitializeDirectX()
	{
		infoManager = new DxgiInfoManager();
		HRESULT hr = S_OK;
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
		if (adapters.size() < 1)
		{
			throw GHWND_EXCEPT(hr);
		}

		// swap chain desc
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = GContext::Get().GetWidth();
		swapChainDesc.BufferDesc.Height = GContext::Get().GetHeight();
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = GContext::Get().GetHWnd();
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


		hr = D3D11CreateDeviceAndSwapChain(
			adapters[0].m_pAdapter, // adapter
			D3D_DRIVER_TYPE_UNKNOWN, // driver type
			NULL, // software
			0, // flags
			NULL, // feature levels
			NULL,//D3D_FEATURE_LEVEL_11_1, // feature level
			D3D11_SDK_VERSION, // sdk version
			&swapChainDesc, // swap chain desc
			m_pSwapChain.GetAddressOf(), // swap chain
			m_pDevice.GetAddressOf(), // device
			NULL, // feature level
			m_pDeviceContext.GetAddressOf()); // device context
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to get backbuffer");
			throw GHWND_EXCEPT(hr);
		}

		ComPtr<ID3D11Resource> backBuffer;
		hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to get backbuffer");
			GHWND_EXCEPT(hr);
		}
		hr = m_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create rtv");
			GHWND_EXCEPT(hr);
		}

		LOG_INFO("DirectX 11 device and swap chain created");

		// Create depth stencil state
		ComPtr<ID3D11DepthStencilState> pDepthStencilState;
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		ComPtr<ID3D11DepthStencilState> pDSState;
		
		GFX_THROW_INFO(m_pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

		// Bind depth state
		m_pDeviceContext->OMSetDepthStencilState(pDSState.Get(), 1u);

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = GContext::Get().GetWidth();
		descDepth.Height = GContext::Get().GetHeight();
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		ComPtr<ID3D11Texture2D> pDepthStencilTexture;
		GFX_THROW_INFO(m_pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencilTexture));

		// Create view of depth stencil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		GFX_THROW_INFO(m_pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), &descDSV, m_pDepthStencilView.GetAddressOf()));

		// Bind depth stencil view to OM
		m_pDeviceContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

		ImGui_ImplDX11_Init(m_pDevice.Get(), m_pDeviceContext.Get());

		m_IsInitialized = true;
	
		return true;
	}

}