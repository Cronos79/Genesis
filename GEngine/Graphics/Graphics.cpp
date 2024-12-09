#include "GEngine/Core/GEnginePCH.h"
#include "Graphics.h"
#include <GEngine/Logger/Log.h>
#include "../Core/GContext.h"

using namespace Microsoft::WRL;
using namespace DirectX;

namespace Genesis
{
	Graphics::Graphics(HWND hWnd, int32 width, int32 height)
	{
		// Create the factory
		ComPtr<IDXGIFactory1> pFactory;
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create DXGIFactory.");
			return;
		}
		
		ComPtr<IDXGIAdapter> pBestAdapter = nullptr;
		SIZE_T maxDedicatedVideoMemory = 0;

		for (UINT i = 0; ; ++i)
		{
			ComPtr<IDXGIAdapter> pAdapter = nullptr;
			if (pFactory->EnumAdapters(i, &pAdapter) == DXGI_ERROR_NOT_FOUND)
			{
				break;
			}

			DXGI_ADAPTER_DESC desc;
			pAdapter->GetDesc(&desc);

			if (desc.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
				if (pBestAdapter)
				{
					pBestAdapter.ReleaseAndGetAddressOf();
				}
				pBestAdapter = pAdapter;
			}
			else
			{
				pAdapter.ReleaseAndGetAddressOf();
			}
		}

		if (!pBestAdapter)
		{
			LOG_ERROR("Failed to find a suitable adapter.");
			return;
		}

		// Swap chain flags for debug
		UINT createDeviceFlags = 0;
#if defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Create device and swap chain
		DXGI_SWAP_CHAIN_DESC scd = {};
		scd.BufferDesc.Width = width;
		scd.BufferDesc.Height = height;
		scd.BufferDesc.RefreshRate.Numerator = 0;
		scd.BufferDesc.RefreshRate.Denominator = 0;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hWnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = createDeviceFlags;		

		hr = D3D11CreateDeviceAndSwapChain(pBestAdapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN, nullptr, 
			createDeviceFlags, nullptr, 0, 
			D3D11_SDK_VERSION, &scd, &m_pSwapChain, &m_pDevice, nullptr, &m_pContext);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create device and swap chain.");
			return;
		}

		// Get the back buffer
		ComPtr<ID3D11Texture2D> pBackBuffer;
		hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to get back buffer.");
			return;
		}

		// Create the render target view		
		hr = m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create render target view.");
			return;
		}

		// Set the render target
		m_pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);

		// Set the viewport
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<FLOAT>(width);
		vp.Height = static_cast<FLOAT>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pContext->RSSetViewports(1, &vp);

		// Init imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		ImGui::StyleColorsDark();
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());

		GContext::Get().SetRunning(true);
		LOG_INFO("Graphics initialized.");
	}
	Graphics::~Graphics()
	{
	}
	void Graphics::BeginFrame(float deltaTime)
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ClearFrame();
	}
	void Graphics::EndFrame(float deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Rendering
		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		m_pSwapChain->Present(1, 0);
	}

	void Graphics::ClearFrame()
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);
	}

}
