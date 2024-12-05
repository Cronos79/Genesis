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
#include "DX11Core.h"
#include <GEngine/Core/GenException.h>
#include "GEngine/Core/GContext.h"
#include <imgui_impl_dx11.h>

#include "Importer.hpp"
#include "postprocess.h"
#include "scene.h"

namespace Genesis
{
	DX11Core::DX11Core()
	{
	}
	DX11Core::~DX11Core()
	{
	}
	void DX11Core::Init()
	{
		infoManager = new DxgiInfoManager();
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferDesc.Width = GContext::Get().GetWidth();
		sd.BufferDesc.Height = GContext::Get().GetHeight();
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 0;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = GContext::Get().GetHWnd();
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		UINT swapCreateFlags = 0u;
#ifdef _DEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT hr = S_OK;
		GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swapCreateFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&sd,
			m_swapChain.GetAddressOf(),
			m_device.GetAddressOf(),
			nullptr,
			m_deviceContext.GetAddressOf()));
		
		ComPtr<ID3D11Resource> backBuffer;
		hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to get backbuffer");
			GHWND_EXCEPT(hr);
		}
		hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
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
		GFX_THROW_INFO(m_device->CreateDepthStencilState(&dsDesc, &pDSState));

		// Bind depth state
		m_deviceContext->OMSetDepthStencilState(pDSState.Get(), 1u);

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
		GFX_THROW_INFO(m_device->CreateTexture2D(&descDepth, nullptr, &pDepthStencilTexture));

		// Create view of depth stencil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		GFX_THROW_INFO(m_device->CreateDepthStencilView(pDepthStencilTexture.Get(), &descDSV, m_depthStencilView.GetAddressOf()));

		// Bind depth stencil view to OM
		m_deviceContext->OMSetRenderTargets(1u, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

		ImGui_ImplDX11_Init(m_device.Get(), m_deviceContext.Get());
	}
	void DX11Core::Shutdown()
	{
	}
	void DX11Core::BeginFrame(float deltaTime)
	{		
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();	
		static float f = 0.0f;
		f += deltaTime;
		const float c = sin(f) / 2.0f + 0.5f;
		ClearBuffer(c, c, 1.0f);
	}
	void DX11Core::EndFrame(float deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Render();	

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// Present the frame
		m_swapChain->Present(1u, 0u);
	}

	void DX11Core::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
	{
		GFX_THROW_INFO_ONLY(m_deviceContext->DrawIndexed(count, 0u, 0u));
	}

	void DX11Core::SetProjection(DirectX::FXMMATRIX proj) noexcept
	{
		m_projection = proj;
	}

	DirectX::XMMATRIX DX11Core::GetProjection() const noexcept
	{
		return m_projection;
	}

	void DX11Core::DrawCube(float angle, float x, float y, float z)
	{
		HRESULT hr;

		struct Vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			} pos;			
		};

		// create vertex buffer (1 2d triangle at center of screen)
		const Vertex vertices[] =
		{
			{ -1.0f, -1.0f, -1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ 1.0f, 1.0f, -1.0f },
			{ -1.0f, -1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f }
		};
		ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		GFX_THROW_INFO(m_device->CreateBuffer(&bd, &sd, &pVertexBuffer));

		// Bind vertex buffer to pipeline
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		m_deviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		// create index buffer
		const unsigned short indices[] =
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};

		ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		GFX_THROW_INFO(m_device->CreateBuffer(&ibd, &isd, &pIndexBuffer));

		// bind index buffer to pipeline
		m_deviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

		// Create constant buffer for transformation matrix
		struct ConstantBuffer
		{
			dx::XMMATRIX transform;
		};
		float aspectRatio = (float)GContext::Get().GetHeight() / (float)GContext::Get().GetWidth();
		const ConstantBuffer cb =
		{
			dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle) *
				dx::XMMatrixRotationX(angle) *
				dx::XMMatrixTranslation(x, y, z + 4.0f) *
				dx::XMMatrixPerspectiveLH(1.0f, aspectRatio, 0.5f, 10.0f))
		};

		ComPtr<ID3D11Buffer> pConstantBuffer;
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(cb);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &cb;
		GFX_THROW_INFO(m_device->CreateBuffer(&cbd, &csd, &pConstantBuffer));

		// bind constant buffer to vertex shader
		m_deviceContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

		struct ConstantBuffer2
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			}face_colors[6];			
		};
		const ConstantBuffer2 cb2 =
		{
			{
				{1.0f, 0.0f, 1.0f, 1.0f},
				{1.0f, 0.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 0.0f, 1.0f},
				{0.0f, 0.0f, 1.0f, 1.0f},
				{1.0f, 1.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 1.0f, 1.0f}
			}
		};
		ComPtr<ID3D11Buffer> pConstantBuffer2;
		D3D11_BUFFER_DESC cbd2 = {};
		cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd2.Usage = D3D11_USAGE_DYNAMIC;
		cbd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd2.MiscFlags = 0u;
		cbd2.ByteWidth = sizeof(cb2);
		cbd2.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd2 = {};
		csd2.pSysMem = &cb2;
		GFX_THROW_INFO(m_device->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));

		// bind constant buffer to pixel shader
		m_deviceContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());

		// create pixel shader
		ComPtr<ID3D11PixelShader> pPixelShader;
		ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
		GFX_THROW_INFO(m_device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

		// bind pixel shader
		m_deviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);


		// create vertex shader
		ComPtr<ID3D11VertexShader> pVertexShader;
		GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
		GFX_THROW_INFO(m_device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

		// bind vertex shader
		m_deviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


		// input (vertex) layout (2d position only)
		ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		GFX_THROW_INFO(m_device->CreateInputLayout(
			ied, (UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout
		));

		// bind vertex layout
		m_deviceContext->IASetInputLayout(pInputLayout.Get());

		// Set primitive topology to triangle list (groups of 3 vertices)
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		// configure viewport
		D3D11_VIEWPORT vp;
		vp.Width = GContext::Get().GetWidth();
		vp.Height = GContext::Get().GetHeight();
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_deviceContext->RSSetViewports(1u, &vp);


		GFX_THROW_INFO_ONLY(m_deviceContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
	}

	void DX11Core::ClearBuffer(float red, float green, float blue) noexcept
	{
		const float color[] = { red, green, blue, 1.0f };
		m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
		m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}
} // namespace Genesis
