#include "genginepch.h"
#include "GEngineD3D12.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include "GEngineLog.h"
#include "GEngineDXDebugLayer.h"

GEngineD3D12::GEngineD3D12(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd)
	: m_Width(width), m_Height(height), m_hInstance(hInstance), m_hWnd(hWnd)
{
	BB_TRACE("D3D12 Starting");
	GEngineDXDebugLayer::Get().Init();
	InIt();
}

GEngineD3D12::~GEngineD3D12()
{
	//Shutdown();
}

bool GEngineD3D12::InIt()
{
	HRESULT hr;

	// Create DXGI Factory
	hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory));
	if (FAILED(hr))
	{
		throw CHWND_EXCEPT(hr);
		return false;
	}

	// Create the Device
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_Device));
	if (FAILED(hr))
	{
		throw CHWND_EXCEPT(hr);
		return false;
	}

	// Create the Command Queue
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = m_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_CmdQueue));
	if (FAILED(hr))
	{
		throw CHWND_EXCEPT(hr);
		return false;
	}

	// Create the Fence
	hr = m_Device->CreateFence(M_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
	if (FAILED(hr))
	{
		throw CHWND_EXCEPT(hr);
		return false;
	}
	
	m_FenceEvent = m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (!m_FenceEvent)
	{
		throw CHWND_LAST_EXCEPT();
		return m_FenceEvent != nullptr;
	}
		
	// Create Command alloc
	hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CmdAllocator));
	if (FAILED(hr))
	{
		throw CHWND_EXCEPT(hr);
		return false;
	}

	// Create Command list
	hr = m_Device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_CmdList));
	if (FAILED(hr))
	{
		throw CHWND_EXCEPT(hr);
		return false;
	}

	// Describe swap chain
	DXGI_SWAP_CHAIN_DESC1 swd{};
	swd.Width = m_Width;
	swd.Height = m_Height;
	swd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swd.Stereo = false;
	swd.SampleDesc.Count = 1;
	swd.SampleDesc.Quality = 0;
	swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swd.BufferCount = SWAP_BUFFER_COUNT;
	swd.Scaling = DXGI_SCALING_STRETCH;
	swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd{};
	sfd.Windowed = true;

	// Create swap chain
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	m_dxgiFactory->CreateSwapChainForHwnd(*m_CmdQueue.GetAddressOf(), m_hWnd, &swd, &sfd, nullptr, swapChain1.GetAddressOf());
	hr = swapChain1->QueryInterface(m_SwapChain.GetAddressOf());
	if (FAILED(hr))
	{
		throw CHWND_EXCEPT(hr);
		return false;
	}

	return true;
}

ID3D12GraphicsCommandList10* GEngineD3D12::InitCommandList()
{
	m_CmdAllocator->Reset();
	m_CmdList->Reset(*m_CmdAllocator.GetAddressOf(), nullptr);
	return *m_CmdList.GetAddressOf();
}

void GEngineD3D12::ExecuteCommandList()
{
	if (SUCCEEDED(m_CmdList->Close()))
	{
		ID3D12CommandList* lists[] = { *m_CmdList.GetAddressOf() };
		m_CmdQueue->ExecuteCommandLists(1, lists);
		SignalAndWait();
	}
}

void GEngineD3D12::Shutdown()
{
	BB_TRACE("D3D12 Shutting down");

	Flush(SWAP_BUFFER_COUNT);

	if (m_FenceEvent)
	{
		CloseHandle(m_FenceEvent);
	}
	
	delete this;
}

void GEngineD3D12::Flush(size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		SignalAndWait();
	}
}

void GEngineD3D12::SignalAndWait()
{
	m_CmdQueue->Signal(*m_Fence.GetAddressOf(), ++M_FenceValue);
	HRESULT hr;
	hr = m_Fence->SetEventOnCompletion(M_FenceValue, m_FenceEvent);
	if (SUCCEEDED(hr))
	{
		if (WaitForSingleObject(m_FenceEvent, 20000) != WAIT_OBJECT_0)
		{
			CHWND_LAST_EXCEPT();
			std::exit(-1);
		}
	}
	else
	{
		//throw CHWND_EXCEPT(hr);
	}	
}

void GEngineD3D12::ResizeSwapChain()
{
	RECT cr;
	if (GetClientRect(m_hWnd, &cr))
	{
		m_Width = cr.right - cr.left;
		m_Height = cr.left - cr.top;

		m_SwapChain->ResizeBuffers(SWAP_BUFFER_COUNT, m_Width, m_Height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
		m_ShouldResize = false;
	}
}

void GEngineD3D12::BeginFrame(float dt)
{
	// Handle window screen locked
	if (m_SwapChainOccluded && m_SwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
	{
		::Sleep(10);
	}
	m_SwapChainOccluded = false;

	if (m_ShouldResize)
	{
		Flush(SWAP_BUFFER_COUNT);
		ResizeSwapChain();
	}
	InitCommandList();	
}

void GEngineD3D12::EndFrame(float dt)
{
	ExecuteCommandList();

	HRESULT hr = m_SwapChain->Present(1, 0);
	m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}

//void GEngineD3D12::BeginRender(float dt)
//{
	//// Handle window screen locked
	//if (m_SwapChainOccluded && m_SwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
	//{
	//	::Sleep(10);
	//}
	//m_SwapChainOccluded = false;

	////// Start the Dear ImGui frame
	//ImGui_ImplDX12_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();
//}

// void GEngineD3D12::EndRender(float dt)
// {
// 	// Rendering
// 	ImGui::Render();
// 
// 	FrameContext* frameCtx = WaitForNextFrameResources();
// 	UINT backBufferIdx = m_SwapChain->GetCurrentBackBufferIndex();
// 	frameCtx->CommandAllocator->Reset();
// 
// 	D3D12_RESOURCE_BARRIER barrier = {};
// 	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
// 	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
// 	barrier.Transition.pResource = *m_mainRenderTargetResource[backBufferIdx].GetAddressOf();
// 	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
// 	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
// 	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
// 	m_d3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
// 	m_d3dCommandList->ResourceBarrier(1, &barrier);
// 
// 	// Render Dear ImGui graphics
// 	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
// 	m_d3dCommandList->ClearRenderTargetView(m_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
// 	m_d3dCommandList->OMSetRenderTargets(1, &m_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
// 	m_d3dCommandList->SetDescriptorHeaps(1, &m_d3dSrvDescHeap);
// 	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), *m_d3dCommandList.GetAddressOf());
// 	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
// 	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
// 	m_d3dCommandList->ResourceBarrier(1, &barrier);
// 	m_d3dCommandList->Close();
// 
// 	m_d3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)m_d3dCommandList.GetAddressOf());
// 
// 	// Update and Render additional Platform Windows
// 	ImGuiIO& io = ImGui::GetIO(); (void)io;
// 	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
// 	{
// 		ImGui::UpdatePlatformWindows();
// 		ImGui::RenderPlatformWindowsDefault(nullptr, (void*)m_d3dCommandList.GetAddressOf());
// 	}
// 
// 	// Present
// 	HRESULT hr = m_SwapChain->Present(1, 0);   // Present with vsync
// 	//HRESULT hr = m_SwapChain->Present(0, 0); // Present without vsync
// 	m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
// 
// 	UINT64 fenceValue = m_fenceLastSignaledValue + 1;
// 	m_d3dCommandQueue->Signal(*m_fence.GetAddressOf(), fenceValue);
// 	m_fenceLastSignaledValue = fenceValue;
// 	frameCtx->FenceValue = fenceValue;
// }
