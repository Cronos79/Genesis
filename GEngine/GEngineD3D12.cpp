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

	// Create rtv heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = SWAP_BUFFER_COUNT;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	hr = m_Device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(m_rtvDescHeap.GetAddressOf()));
	if (FAILED(hr))
	{
		BB_ERROR("CreateDescriptorHeap rtvDesc failed");
		return false;
	}

	// Create handles to view
	auto firstHandle = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
	auto handleSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (size_t i = 0; i < SWAP_BUFFER_COUNT; ++i)
	{
		m_rtvHandles[i] = firstHandle;
		m_rtvHandles[i].ptr += handleSize * i;
	}

	
	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.NumDescriptors = 1;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = m_Device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&m_srvDescHeap));
	if (FAILED(hr))
	{
		BB_ERROR("CreateDescriptorHeap srvDesc failed");
		return false;
	}
	

	// Get buffers
	if (!GetBuffers())
	{
		return false;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	SetDarkThemeColors();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX12_Init(*m_Device.GetAddressOf(), SWAP_BUFFER_COUNT, // #TODO: SWAP_BUFFER_COUNT is this correct or should it be 3
		DXGI_FORMAT_R8G8B8A8_UNORM, *m_srvDescHeap.GetAddressOf(),
		m_srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		m_srvDescHeap->GetGPUDescriptorHandleForHeapStart());

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
		//SignalAndWait(); // #TODO: Check if this needs to be here
	}
}

void GEngineD3D12::Shutdown()
{
	BB_TRACE("D3D12 Shutting down");
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

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
			//CHWND_LAST_EXCEPT();
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
	ReleaseBuffers();

	RECT cr;
	if (GetClientRect(m_hWnd, &cr))
	{
		m_Width = cr.right - cr.left;
		m_Height = cr.left - cr.top;

		m_SwapChain->ResizeBuffers(SWAP_BUFFER_COUNT, m_Width, m_Height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
		m_ShouldResize = false;
	}

	GetBuffers();
}

bool GEngineD3D12::GetBuffers()
{
	HRESULT hr;
	for (UINT i = 0; i < SWAP_BUFFER_COUNT; ++i)
	{
		hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_Buffers[i]));
		if (FAILED(hr))
		{
			BB_ERROR("m_SwapChain get buffer failed");
			return false;
		}

		D3D12_RENDER_TARGET_VIEW_DESC rendViewDesc{};
		rendViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rendViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rendViewDesc.Texture2D.MipSlice = 0;
		rendViewDesc.Texture2D.PlaneSlice = 0;

		m_Device->CreateRenderTargetView(*m_Buffers[i].GetAddressOf(), &rendViewDesc, m_rtvHandles[i]);
	}
	return true;
}

void GEngineD3D12::ReleaseBuffers()
{
	for (size_t i = 0; i < SWAP_BUFFER_COUNT; ++i)
	{
		m_Buffers[i].Reset();
	}
}

void GEngineD3D12::SetDarkThemeColors()
{
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
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

	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	InitCommandList();

	m_CurrentBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();	

 	D3D12_RESOURCE_BARRIER barrier = {};
 	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
 	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
 	barrier.Transition.pResource = *m_Buffers[m_CurrentBufferIndex].GetAddressOf();
 	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
 	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
 	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CmdList->ResourceBarrier(1, &barrier);
}

void GEngineD3D12::EndFrame(float dt)
{
	// 	// Rendering
	ImGui::Render();

	// Render Dear ImGui graphics
	const float clear_color_with_alpha[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
	m_CmdList->ClearRenderTargetView(m_rtvHandles[m_CurrentBufferIndex], clear_color_with_alpha, 0, nullptr);
	m_CmdList->OMSetRenderTargets(1, &m_rtvHandles[m_CurrentBufferIndex], FALSE, nullptr);
	m_CmdList->SetDescriptorHeaps(1, m_srvDescHeap.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), *m_CmdList.GetAddressOf());
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = *m_Buffers[m_CurrentBufferIndex].GetAddressOf();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_CmdList->ResourceBarrier(1, &barrier);

	ExecuteCommandList();	

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(nullptr, (void*)m_CmdList.GetAddressOf());
	}

	// Present 
	//HRESULT hr = m_SwapChain->Present(0, 0); // Present without vsync
	HRESULT hr = m_SwapChain->Present(1, 0); // Present with vsync	
	m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);	

	SignalAndWait();
}