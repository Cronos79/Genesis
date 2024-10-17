#pragma once

#include "WinInclude.h"
#include <d3d12.h>
#include <dxgi1_6.h>

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

struct FrameContext
{
	ID3D12CommandAllocator* CommandAllocator;
	UINT64                  FenceValue;
};

class GEngineD3D12
{
public:
	GEngineD3D12(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd);
	GEngineD3D12(const GEngineD3D12& rhs) = delete;
	GEngineD3D12& operator=(const GEngineD3D12& rhs) = delete;
	~GEngineD3D12();

	void Update(float dt);

	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void WaitForLastSubmittedFrame();
	FrameContext* WaitForNextFrameResources();

private:
	int m_Width;
	int m_Height;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	#define NUM_FRAMES_IN_FLIGHT 3
	FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
	UINT                         g_frameIndex = 0;

	#define NUM_BACK_BUFFERS 3
	ID3D12Device* g_pd3dDevice = nullptr;
	ID3D12DescriptorHeap* g_pd3dRtvDescHeap = nullptr;
	ID3D12DescriptorHeap* g_pd3dSrvDescHeap = nullptr;
	ID3D12CommandQueue* g_pd3dCommandQueue = nullptr;
	ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
	ID3D12Fence* g_fence = nullptr;
	HANDLE g_fenceEvent = nullptr;
	UINT64 g_fenceLastSignaledValue = 0;
	IDXGISwapChain3* g_pSwapChain = nullptr;
	bool g_SwapChainOccluded = false;
	HANDLE g_hSwapChainWaitableObject = nullptr;
	ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
};