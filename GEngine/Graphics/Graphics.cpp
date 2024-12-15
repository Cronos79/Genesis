#include "Graphics.h"
#include "GEngine/Graphics/GraphicsError.h"
#include <GEngine/src/log/Log.h> 
#include <GEngine/src/utl/String.h> 

#include <cmath> 
#include <numbers> 
#include <ranges>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "../Core/GContext.h"

namespace Genesis
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;
	using namespace chil;
	using namespace chil::app;
	namespace rn = std::ranges;
	namespace vi = rn::views;

	

	Graphics::Graphics()
	{
	}
	void Graphics::Initialize(chil::win::IWindow& window)
	{
		// init COM
		CoInitializeEx(nullptr, COINIT_MULTITHREADED) >> chk;
		SetupDebugLayer();
		CreateFactory();
		CreateDevice();
		CreateCommandObjects();
		CreateSwapChain(window);
		CreateRenderTargets();
		CreateDepthBuffer();
		CreateFence();
		CreateDescriptorHeaps();
		SetScissorAndViewport();	

		InitImGui(window);
	}

	void Graphics::BeginFrame(float deltaTime)
	{
		// advance back buffer
		curBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
		ResetCommandList();

		// transition buffer resource to render target state 
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffers[curBackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &barrier);

		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv{
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		(INT)curBackBufferIndex, rtvDescriptorSize };

		// calculate clear color 
		const FLOAT clearColor[] = {
			sin(2.f * t + 1.f) / 2.f + .5f,
			sin(3.f * t + 2.f) / 2.f + .5f,
			sin(5.f * t + 3.f) / 2.f + .5f,
			1.0f
		};
		// clear rtv 
		commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		// clear the depth buffer 
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

		// configure RS 
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorRect);

		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void Graphics::EndFrame(float deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Rendering
		ImGui::Render();

		/*const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv{
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		(INT)curBackBufferIndex, rtvDescriptorSize };*/
		//commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
		//commandList->SetDescriptorHeaps(1, &srvHeap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

		// prepare buffer for presentation by transitioning to present state
		{
			const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffers[curBackBufferIndex].Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);
		}

		CloseAndExecuteCommandList();

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// insert fence to mark command list completion 
		commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
		// present frame 
		swapChain->Present(1, 0) >> chk;

		SignalAndWait();

		// update simulation time 
		t += step;
	}

	void Graphics::SetupDebugLayer()
	{
		// enable the software debug layer for d3d12 
		{
			ComPtr<ID3D12Debug1> debugController;
			D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) >> chk;
			debugController->EnableDebugLayer();
			debugController->SetEnableGPUBasedValidation(true);
		}
	}

	void Graphics::CreateFactory()
	{
		// dxgi factory 
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory)) >> chk;
	}

	void Graphics::CreateDevice()
	{
		// device
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)) >> chk;
	}

	void Graphics::CreateCommandObjects()
	{
		// command queue
		const D3D12_COMMAND_QUEUE_DESC desc = {
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0,
		};
		device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)) >> chk;

		// command allocator
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator)) >> chk;

		// command list
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)) >> chk;
		// initially close the command list so it can be reset at top of draw loop 
		commandList->Close() >> chk;
	}

	void Graphics::CreateSwapChain(chil::win::IWindow& window)
	{
		// swap chain
		const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
			.Width = (UINT)GContext::Get().GetWidth(),
			.Height = (UINT)GContext::Get().GetHeight(),
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.Stereo = FALSE,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = bufferCount,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = 0,
		};
		ComPtr<IDXGISwapChain1> swapChain1;
		dxgiFactory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			window.GetHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1) >> chk;
		swapChain1.As(&swapChain) >> chk;
	}

	void Graphics::CreateRenderTargets()
	{
		// rtv descriptor heap
		const D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = bufferCount,
		};
		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvDescriptorHeap)) >> chk;

		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// rtv descriptors and buffer references
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (int i = 0; i < bufferCount; i++)
		{
			swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])) >> chk;
			device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);
		}
	}

	void Graphics::CreateDepthBuffer()
	{
		// depth buffer 
		const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			GContext::Get().GetWidth(), GContext::Get().GetHeight(),
			1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		const D3D12_CLEAR_VALUE clearValue = {
			.Format = DXGI_FORMAT_D32_FLOAT,
			.DepthStencil = { 1.0f, 0 },
		};
		device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&depthBuffer)) >> chk;

		// dsv descriptor heap
		const D3D12_DESCRIPTOR_HEAP_DESC desc1 = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1,
		};
		device->CreateDescriptorHeap(&desc1, IID_PPV_ARGS(&dsvDescriptorHeap)) >> chk;

		// dsv and handle 
		dsvHandle = {
			dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

		device->CreateDepthStencilView(depthBuffer.Get(), nullptr, dsvHandle);
	}

	void Graphics::CreateFence()
	{
		// fence
		device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) >> chk;

		// fence signalling event
		fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!fenceEvent)
		{
			GetLastError() >> chk;
			throw std::runtime_error{ "Failed to create fence event" };
		}
	}

	void Graphics::InitImGui(chil::win::IWindow& window)
	{
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

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(window.GetHandle());

		// Setup Platform/Renderer bindings
		ImGui_ImplDX12_Init(device.Get(), bufferCount,
			DXGI_FORMAT_R8G8B8A8_UNORM, srvHeap.Get(),
			srvHeap->GetCPUDescriptorHandleForHeapStart(),
			srvHeap->GetGPUDescriptorHandleForHeapStart());
	}

	void Graphics::CreateDescriptorHeaps()
	{
		// descriptor heap for the shader resource view 
		{
			const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				.NumDescriptors = 1, // #TODO: Was 1 in the tutorial, but I don't know why
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
			};
			device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)) >> chk;
		}		
	}

	void Graphics::ResetCommandList()
	{
		// reset command list and allocator  
		commandAllocator->Reset() >> chk;
		commandList->Reset(commandAllocator.Get(), nullptr) >> chk;
	}

	void Graphics::CloseAndExecuteCommandList()
	{
		// close command list  
		commandList->Close() >> chk;
		// submit command list to queue as array with single element 
		ID3D12CommandList* const commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
	}

	void Graphics::SignalAndWait()
	{
		// insert fence to detect when upload is complete 
		commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
		fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
		if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
		{
			GetLastError() >> chk;
		}
	}

	void Graphics::SetScissorAndViewport()
	{
		// define scissor rect
		scissorRect.top = 0;
		scissorRect.left = 0;
		scissorRect.right = LONG_MAX;
		scissorRect.bottom = LONG_MAX;

		// define viewport 
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = float(GContext::Get().GetWidth());
		viewport.Height = float(GContext::Get().GetHeight());
		//viewport.MinDepth = 0;
		//viewport.MaxDepth = 1;
	}	

	void Graphics::Shutdown()
	{
		// wait for queue to become completely empty (2 seconds max) 
		commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
		fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
		if (WaitForSingleObject(fenceEvent, 2000) == WAIT_FAILED)
		{
			GetLastError() >> chk;
		}
	}

}