#include "GEngine/Core/GEnginePCH.h"
#include "Graphics12.h"
#include <GEngine/Logger/Log.h>
#include <GEngine/Core/GenException.h>
#include "GEngine/Core/GContext.h"
#include "GEngine/Graphics/GraphicsError.h"

namespace Genesis
{
	Graphics12::Graphics12(HWND hWnd, int32 width, int32 height)
	{
		// init COM
		CoInitializeEx(nullptr, COINIT_MULTITHREADED) >> chk;

		SetupDebugLayer();
		if (CreateDevice())
		{
			CreateCommandQueue();
			CreateSwapChain(hWnd, width, height);
			CreateDescriptorHeaps();
			CreateRenderTargetViews();
			CreateCommandObjects();
			CreateFence();
			CheckFeatureSupport();

			GContext::Get().SetRunning(true);
		}
	}

	Graphics12::~Graphics12()
	{

	}

	void Graphics12::BeginFrame(float deltaTime)
	{
		m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
		m_pCommandAllocator->Reset() >> chk;
		m_pCommandList->Reset(m_pCommandAllocator.Get(), nullptr) >> chk;

		// Barrier
		const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_pRenderTargets[m_CurrentBackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_pCommandList->ResourceBarrier(1, &barrier);

		ClearFrame();
	}

	void Graphics12::EndFrame(float deltaTime)
	{
		// Barrier
		const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_pRenderTargets[m_CurrentBackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_pCommandList->ResourceBarrier(1, &barrier);

		// Close the list
		m_pCommandList->Close();

		// Execute the command list
		ID3D12CommandList* ppCommandLists[] = { m_pCommandList.Get() };
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Signal
		const uint64 fence = m_FenceValue;
		m_pCommandQueue->Signal(m_pFence.Get(), fence) >> chk;
		m_FenceValue++;

		// Present
		m_pSwapChain->Present(1, 0) >> chk;

		// Fence event
		if (m_pFence->GetCompletedValue() < fence)
		{
			m_pFence->SetEventOnCompletion(fence, m_hFenceEvent) >> chk;
			WaitForSingleObject(m_hFenceEvent, INFINITE);
		}
	}

	void Graphics12::Shutdown()
	{

	}

	void Graphics12::SetupDebugLayer()
	{
#ifdef _DEBUG
		ComPtr<ID3D12Debug1> debugController;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) >> chk;
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(true);
#endif
	}

	bool Graphics12::CreateDevice()
	{
		// dxgi factory 		
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_pDxgiFactory)) >> chk;

		ComPtr<IDXGIAdapter4> adapter = GetHardwareAdapter(m_pDxgiFactory.Get());
		if (adapter == nullptr)
		{
			LOG_ERROR("Failed to get hardware adapter.");
			return false;
		}

		ComPtr<ID3D12Device4> device4;
		D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device4)) >> chk;

		//D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device4)) >> chk;
		// Set m_pDevice
		device4.As(&m_pDevice);

		return true;
	}

	bool Graphics12::CreateCommandQueue()
	{
		// command queue
		const D3D12_COMMAND_QUEUE_DESC desc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0,
		};
		m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pCommandQueue)) >> chk;
		return true;
	}

	bool Graphics12::CreateSwapChain(HWND hWnd, int32 width, int32 height)
	{
		const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
				.Width = (UINT)width,
				.Height = (UINT)height,
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.Stereo = FALSE,
				.SampleDesc = {
					.Count = 1,
					.Quality = 0
				},
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = (UINT)m_NumBuffers,
				.Scaling = DXGI_SCALING_STRETCH,
				.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
				.Flags = 0,
		};
		ComPtr<IDXGISwapChain1> swapChain1;
		m_pDxgiFactory->CreateSwapChainForHwnd(
			m_pCommandQueue.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1) >> chk;
		swapChain1.As(&m_pSwapChain) >> chk;

		return true;
	}

	bool Graphics12::CreateDescriptorHeaps()
	{
		// rtv descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = m_NumBuffers;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRtvDescHeap)) >> chk;

		m_RtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// srv descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = m_NumBuffers;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSrvDescHeap)) >> chk;

		m_SrvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return true;
	}

	bool Graphics12::CreateRenderTargetViews()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRtvDescHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < m_NumBuffers; i++)
		{
			m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i])) >> chk;
			m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(m_RtvDescriptorSize);
		}
		return true;
	}

	bool Graphics12::CreateCommandObjects()
	{
		// command allocator
		m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator)) >> chk;

		// command list
		m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_pCommandList)) >> chk;
		// Close the list
		m_pCommandList->Close();

		return true;
	}

	bool Graphics12::CreateFence()
	{
		// Create fence
		m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)) >> chk;

		// Create fence event
		m_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_hFenceEvent == nullptr)
		{
			GetLastError() >> chk;
			LOG_ERROR("Failed to create fence event.");
			return false;
		}

		return true;
	}

	void Graphics12::CheckFeatureSupport()
	{
		INFOMAN();
		// Check feature support
		D3D12_FEATURE_DATA_D3D12_OPTIONS featureData = {};
		GFX_THROW_INFO(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &featureData, sizeof(featureData)));

		// Check for 4x MSAA quality levels
		DXGI_SAMPLE_DESC msaaQualityLevels;
		msaaQualityLevels.Count = 4;
		msaaQualityLevels.Quality = 0;

		if (SUCCEEDED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevels, sizeof(msaaQualityLevels))))
		{
			// Get if MSAA4 is enabled
			// #TODO: Check if MSAA4 is supported
		}

		// Check for ray tracing support
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureData5 = {};
		if (SUCCEEDED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureData5, sizeof(featureData5))))
		{
			if (featureData5.RaytracingTier < D3D12_RAYTRACING_TIER_1_0)
			{
				LOG_ERROR("Ray tracing not supported.");
				return;
			}
			else
			{
				m_bRayTracingSupported = true;
				LOG_INFO("Ray tracing supported.");
			}
		}
	}

	void Graphics12::WaitForPreviousFrame()
	{
		// Wait for previous frame
		const uint64 fence = m_FenceValue;
		m_pCommandQueue->Signal(m_pFence.Get(), fence) >> chk;
		m_FenceValue++;

		if (m_pFence->GetCompletedValue() < fence)
		{
			m_pFence->SetEventOnCompletion(fence, m_hFenceEvent) >> chk;
			WaitForSingleObject(m_hFenceEvent, INFINITE);
		}
	}

	void Graphics12::ClearFrame()
	{
		// Clear the render target
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			m_pRtvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			m_CurrentBackBufferIndex, m_RtvDescriptorSize);
		m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	ComPtr<IDXGIAdapter4> Graphics12::GetHardwareAdapter(IDXGIFactory4* factory)
	{
		ComPtr<IDXGIAdapter1> adapter;

		// Check for hardware adapter
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}
			// Check if the adapter supports Direct3D 12, but don't create the actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}

		ComPtr<IDXGIAdapter4> adapter4;
		adapter.As(&adapter4);

		return adapter4;
	}

}

