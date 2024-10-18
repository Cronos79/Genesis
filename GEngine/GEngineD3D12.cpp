#include "genginepch.h"
#include "GEngineD3D12.h"
#include "GEngineException.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"


GEngineD3D12::GEngineD3D12(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd)
	: m_Width(width), m_Height(height), m_hInstance(hInstance), m_hWnd(hWnd)
{
	// Initialize Direct3D
	if (!CreateDeviceD3D(m_hWnd))
	{
		CleanupDeviceD3D();
		return;
	}
}

GEngineD3D12::~GEngineD3D12()
{
	
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
void GEngineD3D12::BeginRender(float dt)
{
	// Handle window screen locked
	if (m_SwapChainOccluded && m_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
	{
		::Sleep(10);
	}
	m_SwapChainOccluded = false;

	//// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void GEngineD3D12::EndRender(float dt)
{
	// Rendering
	ImGui::Render();

	FrameContext* frameCtx = WaitForNextFrameResources();
	UINT backBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
	frameCtx->CommandAllocator->Reset();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = *m_mainRenderTargetResource[backBufferIdx].GetAddressOf();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
	m_pd3dCommandList->ResourceBarrier(1, &barrier);

	// Render Dear ImGui graphics
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	m_pd3dCommandList->ClearRenderTargetView(m_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
	m_pd3dCommandList->OMSetRenderTargets(1, &m_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
	m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dSrvDescHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), *m_pd3dCommandList.GetAddressOf());
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_pd3dCommandList->ResourceBarrier(1, &barrier);
	m_pd3dCommandList->Close();

	m_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)m_pd3dCommandList.GetAddressOf());

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(nullptr, (void*)m_pd3dCommandList.GetAddressOf());
	}

	// Present
	HRESULT hr = m_pSwapChain->Present(1, 0);   // Present with vsync
	//HRESULT hr = m_pSwapChain->Present(0, 0); // Present without vsync
	m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

	UINT64 fenceValue = m_fenceLastSignaledValue + 1;
	m_pd3dCommandQueue->Signal(*m_fence.GetAddressOf(), fenceValue);
	m_fenceLastSignaledValue = fenceValue;
	frameCtx->FenceValue = fenceValue;
}

bool GEngineD3D12::CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = NUM_BACK_BUFFERS;
		sd.Width = 0;
		sd.Height = 0;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}

	// [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
	ID3D12Debug* pdx12Debug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
		pdx12Debug->EnableDebugLayer();
#endif

	// Create device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_d3dDevice)) != S_OK)
		return false;

	// [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
	if (pdx12Debug != nullptr)
	{
		ID3D12InfoQueue* pInfoQueue = nullptr;
		m_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		pInfoQueue->Release();
		pdx12Debug->Release();
	}
#endif

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = NUM_BACK_BUFFERS;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		if (m_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dRtvDescHeap)) != S_OK)
			return false;

		SIZE_T rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			m_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (m_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dSrvDescHeap)) != S_OK)
			return false;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 1;
		if (m_d3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pd3dCommandQueue)) != S_OK)
			return false;
	}

	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_frameContext[i].CommandAllocator)) != S_OK)
			return false;

	if (m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&m_pd3dCommandList)) != S_OK ||
		m_pd3dCommandList->Close() != S_OK)
		return false;

	if (m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)) != S_OK)
		return false;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
		return false;

	{
		IDXGIFactory4* dxgiFactory = nullptr;
		IDXGISwapChain1* swapChain1 = nullptr;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
			return false;
		if (dxgiFactory->CreateSwapChainForHwnd(*m_pd3dCommandQueue.GetAddressOf(), hWnd, &sd, nullptr, nullptr, &swapChain1) != S_OK)
			return false;
		if (swapChain1->QueryInterface(IID_PPV_ARGS(&m_pSwapChain)) != S_OK)
			return false;
		swapChain1->Release();
		dxgiFactory->Release();
		m_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		m_hSwapChainWaitableObject = m_pSwapChain->GetFrameLatencyWaitableObject();
	}

	CreateRenderTarget();
	return true;
}

void GEngineD3D12::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (m_pSwapChain) { m_pSwapChain->SetFullscreenState(false, nullptr); m_pSwapChain->Release(); m_pSwapChain = nullptr; }
	if (m_hSwapChainWaitableObject != nullptr) { CloseHandle(m_hSwapChainWaitableObject); }
	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (m_frameContext[i].CommandAllocator) { m_frameContext[i].CommandAllocator->Release(); m_frameContext[i].CommandAllocator = nullptr; }
	if (m_pd3dCommandQueue) { m_pd3dCommandQueue->Release(); m_pd3dCommandQueue = nullptr; }
	if (m_pd3dCommandList) { m_pd3dCommandList->Release(); m_pd3dCommandList = nullptr; }
	if (m_pd3dRtvDescHeap) { m_pd3dRtvDescHeap->Release(); m_pd3dRtvDescHeap = nullptr; }
	if (m_pd3dSrvDescHeap) { m_pd3dSrvDescHeap->Release(); m_pd3dSrvDescHeap = nullptr; }
	if (m_fence) { m_fence->Release(); m_fence = nullptr; }
	if (m_fenceEvent) { CloseHandle(m_fenceEvent); m_fenceEvent = nullptr; }
	if (m_d3dDevice) { m_d3dDevice->Release(); m_d3dDevice = nullptr; }

#ifdef DX12_ENABLE_DEBUm_LAYER
	IDXGIDebug1* pDebug = nullptr;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
	{
		pDebug->ReportLiveObjects(DXGI_DEBUm_ALL, DXGI_DEBUm_RLO_SUMMARY);
		pDebug->Release();
	}
#endif
}

void GEngineD3D12::CreateRenderTarget()
{
	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
	{
		ID3D12Resource* pBackBuffer = nullptr;
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		m_d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, m_mainRenderTargetDescriptor[i]);
		m_mainRenderTargetResource[i] = pBackBuffer;
	}
}

void GEngineD3D12::CleanupRenderTarget()
{
	WaitForLastSubmittedFrame();

	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		if (m_mainRenderTargetResource[i]) { m_mainRenderTargetResource[i]->Release(); m_mainRenderTargetResource[i] = nullptr; }
}

void GEngineD3D12::WaitForLastSubmittedFrame()
{
	FrameContext* frameCtx = &m_frameContext[m_frameIndex % NUM_FRAMES_IN_FLIGHT];

	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue == 0)
		return; // No fence was signaled

	frameCtx->FenceValue = 0;
	if (m_fence->GetCompletedValue() >= fenceValue)
		return;

	m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
	WaitForSingleObject(m_fenceEvent, INFINITE);
}

void GEngineD3D12::OnResize()
{
	assert(m_d3dDevice);
	//assert(m_SwapChain);
	//assert(m_DirectCmdListAlloc);

	//// Flush before changing any resources.
	//FlushCommandQueue();

	//ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	//// Release the previous resources we will be recreating.
	//for (int i = 0; i < SwapChainBufferCount; ++i)
	//	mSwapChainBuffer[i].Reset();
	//mDepthStencilBuffer.Reset();

	//// Resize the swap chain.
	//ThrowIfFailed(mSwapChain->ResizeBuffers(
	//	SwapChainBufferCount,
	//	mClientWidth, mClientHeight,
	//	mBackBufferFormat,
	//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	//mCurrBackBuffer = 0;

	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	//for (UINT i = 0; i < SwapChainBufferCount; i++)
	//{
	//	ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
	//	md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
	//	rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	//}

	//// Create the depth/stencil buffer and view.
	//D3D12_RESOURCE_DESC depthStencilDesc;
	//depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//depthStencilDesc.Alignment = 0;
	//depthStencilDesc.Width = mClientWidth;
	//depthStencilDesc.Height = mClientHeight;
	//depthStencilDesc.DepthOrArraySize = 1;
	//depthStencilDesc.MipLevels = 1;

	//// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	//// the depth buffer.  Therefore, because we need to create two views to the same resource:
	////   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	////   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	//// we need to create the depth buffer resource with a typeless format.  
	//depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	//depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	//depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	//depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//D3D12_CLEAR_VALUE optClear;
	//optClear.Format = mDepthStencilFormat;
	//optClear.DepthStencil.Depth = 1.0f;
	//optClear.DepthStencil.Stencil = 0;
	//ThrowIfFailed(md3dDevice->CreateCommittedResource(
	//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	//	D3D12_HEAP_FLAG_NONE,
	//	&depthStencilDesc,
	//	D3D12_RESOURCE_STATE_COMMON,
	//	&optClear,
	//	IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

	//// Create descriptor to mip level 0 of entire resource using the format of the resource.
	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	//dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//dsvDesc.Format = mDepthStencilFormat;
	//dsvDesc.Texture2D.MipSlice = 0;
	//md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	//// Transition the resource from its initial state to be used as a depth buffer.
	//mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
	//	D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	//// Execute the resize commands.
	//ThrowIfFailed(mCommandList->Close());
	//ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	//mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	//// Wait until resize is complete.
	//FlushCommandQueue();

	//// Update the viewport transform to cover the client area.
	//mScreenViewport.TopLeftX = 0;
	//mScreenViewport.TopLeftY = 0;
	//mScreenViewport.Width = static_cast<float>(mClientWidth);
	//mScreenViewport.Height = static_cast<float>(mClientHeight);
	//mScreenViewport.MinDepth = 0.0f;
	//mScreenViewport.MaxDepth = 1.0f;

	//mScissorRect = { 0, 0, mClientWidth, mClientHeight };
}

FrameContext* GEngineD3D12::WaitForNextFrameResources()
{
	UINT nextFrameIndex = m_frameIndex + 1;
	m_frameIndex = nextFrameIndex;

	HANDLE waitableObjects[] = { m_hSwapChainWaitableObject, nullptr };
	DWORD numWaitableObjects = 1;

	FrameContext* frameCtx = &m_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue != 0) // means no fence was signaled
	{
		frameCtx->FenceValue = 0;
		m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
		waitableObjects[1] = m_fenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	return frameCtx;
}

