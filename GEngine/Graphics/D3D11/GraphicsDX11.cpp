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
		bool result = InitializeDirectX();
		if (!result)
		{
			LOG_ERROR("Failed to initialize DirectX 11");
			return false;
		}

		result = InitializeShaders();
		if (!result)
		{
			LOG_ERROR("Failed to initialize shaders");
			return false;
		}

		result = InitializeScene();
		if (!result)
		{
			LOG_ERROR("Failed to initialize scene");
			return false;
		}

		LOG_INFO("Graphics initialized");
		GContext::Get().SetRunning(true);
		return true;
	}
	void GraphicsDX11::Shutdown()
	{
		ImGui_ImplDX11_Shutdown();
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		LOG_INFO("Graphics shutdown");
	}

	void GraphicsDX11::BeginFrame(float deltaTime)
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static float f = 0.0f;
		f += deltaTime;
		const float c = sin(f) / 2.0f + 0.5f;
		ClearBuffer(0.2f, 0.2f, 1.0f);
	}

	void GraphicsDX11::EndFrame(float deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Render();


		m_pDeviceContext->IASetInputLayout(m_VertexShader.GetInputLayout());
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pDeviceContext->RSSetState(m_pRasterizerState.Get());
		m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 1u);
		m_pDeviceContext->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xFFFFFFFF);
		m_pDeviceContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

		m_pDeviceContext->VSSetShader(m_VertexShader.GetShader(), nullptr, 0);
		m_pDeviceContext->PSSetShader(m_PixelShader.GetShader(), nullptr, 0);
		
		UINT offset = 0;

		// Update constant buffer
		dx::XMMATRIX world = dx::XMMatrixIdentity();
		m_CB_VS_VertexShader.data.mat = world * m_Camera.GetViewMatrix() * m_Camera.GetProjectionMatrix();
		m_CB_VS_VertexShader.data.mat = dx::XMMatrixTranspose(m_CB_VS_VertexShader.data.mat);
		if (!m_CB_VS_VertexShader.ApplyChanges()) return;
		m_pDeviceContext->VSSetConstantBuffers(0u, 1u, m_CB_VS_VertexShader.GetAddressOf());

		// Update pixel shader constant buffer
		m_CB_PS_PixelShader.data.alpha = m_Alpha;
		if (!m_CB_PS_PixelShader.ApplyChanges()) return;
		m_pDeviceContext->PSSetConstantBuffers(0u, 1u, m_CB_PS_PixelShader.GetAddressOf());

		m_pDeviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), m_VertexBuffer.StridePtr(), &offset);
		m_pDeviceContext->IASetIndexBuffer(m_IndicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		m_pDeviceContext->DrawIndexed(m_IndicesBuffer.BufferSize(), 0, 0);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		m_pSwapChain->Present(m_vsync, 0u);
	}

	bool GraphicsDX11::InitializeDirectX()
	{
		try
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

			UINT swapCreateFlags = 0u;
#ifdef _DEBUG
			swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
				adapters[0].m_pAdapter, // adapter
				D3D_DRIVER_TYPE_UNKNOWN, // driver type
				nullptr, // software
				swapCreateFlags, // flags
				nullptr, // feature levels
				NULL,//D3D_FEATURE_LEVEL_11_1, // feature level
				D3D11_SDK_VERSION, // sdk version
				&swapChainDesc, // swap chain desc
				m_pSwapChain.GetAddressOf(), // swap chain
				m_pDevice.GetAddressOf(), // device
				nullptr, // feature level
				m_pDeviceContext.GetAddressOf()
			)); // device context

			ComPtr<ID3D11Resource> backBuffer;
			GFX_THROW_INFO(m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

			GFX_THROW_INFO(m_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));

			// Create depth stencil state
			ComPtr<ID3D11DepthStencilState> pDepthStencilState;
			D3D11_DEPTH_STENCIL_DESC dsDesc = {};
			dsDesc.DepthEnable = TRUE;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS; // #NOTE: LESS_EQUAL will also replace if the pixel is the same

			GFX_THROW_INFO(m_pDevice->CreateDepthStencilState(&dsDesc, m_pDepthStencilState.GetAddressOf()));

			// Create the viewport
			D3D11_VIEWPORT vp = {};
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			vp.Width = GContext::Get().GetWidth();
			vp.Height = GContext::Get().GetHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;

			m_pDeviceContext->RSSetViewports(1u, &vp);

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

			//Create rasterizer state
			D3D11_RASTERIZER_DESC rasterizerDesc = {};
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
			rasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
			rasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			GFX_THROW_INFO(m_pDevice->CreateRasterizerState(&rasterizerDesc, m_pRasterizerState.GetAddressOf()));

			// Create rasterizer state for culling front face
			D3D11_RASTERIZER_DESC rasterizerDesc_CullFront = {};
			rasterizerDesc_CullFront.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc_CullFront.CullMode = D3D11_CULL_FRONT;
			GFX_THROW_INFO(m_pDevice->CreateRasterizerState(&rasterizerDesc_CullFront, m_pRasterizerState_CullFront.GetAddressOf()));

			// Create blend state
			D3D11_BLEND_DESC blendDesc = {};

			D3D11_RENDER_TARGET_BLEND_DESC rtbd = {};
			rtbd.BlendEnable = true;
			rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rtbd.BlendOp = D3D11_BLEND_OP_ADD;
			rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
			rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
			rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			blendDesc.RenderTarget[0] = rtbd;

			GFX_THROW_INFO(m_pDevice->CreateBlendState(&blendDesc, m_pBlendState.GetAddressOf()));

			// Bind depth stencil view to OM
			m_pDeviceContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

			// Create sampler state
			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			GFX_THROW_INFO(m_pDevice->CreateSamplerState(&samplerDesc, m_pSamplerState.GetAddressOf()));

			ImGui_ImplDX11_Init(m_pDevice.Get(), m_pDeviceContext.Get());
		}
		catch (GenException* e)
		{
			LOG_ERROR(e->what());
			return false;
		}
	
		LOG_INFO("DirectX 11 Initialized");
		return true;
	}

	bool GraphicsDX11::InitializeShaders()
	{
		std::wstring shaderFolder;
#pragma region ShaderFolder
		if (IsDebuggerPresent())
		{
#ifdef _DEBUG
			shaderFolder = L"..\\Build\\Bin\\Debug\\";
#else
			shaderFolder = L"..\\Build\\Bin\\Release\\";
#endif
		}
#pragma endregion
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // D3D11_APPEND_ALIGNED_ELEMENT
		};
		UINT numElements = ARRAYSIZE(ied);

		if (!m_VertexShader.Initialize(m_pDevice, shaderFolder + L"VertexShader.cso", ied, numElements))
		{
			LOG_ERROR("Failed to initialize vertex shader");
			return false;
		}

		if (!m_PixelShader.Initialize(m_pDevice, shaderFolder + L"PixelShader.cso"))
		{
			LOG_ERROR("Failed to initialize pixel shader");
			return false;
		}

		LOG_INFO("Shaders initialized");
		return true;
	}

	bool GraphicsDX11::InitializeScene()
	{
		// Create vertex buffer
		Vertex vertices[] =
		{
			Vertex(-0.5f, -0.5f, -0.5f, 1.0f,	0.0f, 1.0f), //FRONT Bottom Left   - [0]
			Vertex(-0.5f,  0.5f, -0.5f, 1.0f,	0.0f, 0.0f), //FRONT Top Left      - [1]
			Vertex(0.5f,   0.5f, -0.5f, 1.0f,	1.0f, 0.0f), //FRONT Top Right     - [2]
			Vertex(0.5f,  -0.5f, -0.5f, 1.0f,	1.0f, 1.0f), //FRONT Bottom Right   - [3]
			Vertex(-0.5f, -0.5f,  0.5f, 1.0f,	0.0f, 1.0f), //BACK Bottom Left   - [4]
			Vertex(-0.5f,  0.5f,  0.5f, 1.0f,	0.0f, 0.0f), //BACK Top Left      - [5]
			Vertex(0.5f,   0.5f,  0.5f, 1.0f,	1.0f, 0.0f), //BACK Top Right     - [6]
			Vertex(0.5f,  -0.5f,  0.5f, 1.0f,	1.0f, 1.0f), //BACK Bottom Right   - [7]
		};

		HRESULT hr;
		GFX_THROW_INFO(m_VertexBuffer.Initialize(m_pDevice.Get(), vertices, ARRAYSIZE(vertices)));

		DWORD indices[] =
		{
			0, 1, 2, //FRONT
			0, 2, 3, //FRONT
			4, 7, 6, //BACK 
			4, 6, 5, //BACK
			3, 2, 6, //RIGHT SIDE
			3, 6, 7, //RIGHT SIDE
			4, 5, 1, //LEFT SIDE
			4, 1, 0, //LEFT SIDE
			1, 5, 6, //TOP
			1, 6, 2, //TOP
			0, 3, 7, //BOTTOM
			0, 7, 4, //BOTTOM
		};	

		GFX_THROW_INFO(m_IndicesBuffer.Initialize(m_pDevice.Get(), indices, ARRAYSIZE(indices)));

		GFX_THROW_INFO(m_CB_VS_VertexShader.Initialize(m_pDevice.Get(), m_pDeviceContext.Get()));

		GFX_THROW_INFO(m_CB_PS_PixelShader.Initialize(m_pDevice.Get(), m_pDeviceContext.Get()));

		m_Camera.SetPosition(0.0f, 0.0f, -2.0f);
		float aspectRatio = (float)GContext::Get().GetWidth() / (float)GContext::Get().GetHeight();
		m_Camera.SetProjectionValues(90.0f, aspectRatio, 0.1f, 1000.0f);

		LOG_INFO("Scene initialized");
		return true;
	}

	void GraphicsDX11::ClearBuffer(float red, float green, float blue) noexcept
	{
		const float color[] = { red, green, blue, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

}