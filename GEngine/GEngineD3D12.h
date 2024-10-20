#pragma once

#include "WinInclude.h"
#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#define NUM_FRAMES_IN_FLIGHT 3
#define NUM_BACK_BUFFERS 3

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

	void BeginRender(float dt);
	void EndRender(float dt);

	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void WaitForLastSubmittedFrame();
	void OnResize();
	FrameContext* WaitForNextFrameResources();

	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return m_d3dDevice; }
	ID3D12DescriptorHeap* GetSrvDescHeap() { return m_d3dSrvDescHeap; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return m_d3dCommandList; }

private:
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	ID3D12DescriptorHeap* m_d3dRtvDescHeap = nullptr;
	ID3D12DescriptorHeap* m_d3dSrvDescHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3dCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_d3dCommandList;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;	
	Microsoft::WRL::ComPtr<ID3D12Resource> m_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};	
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer; // #TODO: Add me 
	//Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount]; // #TODO: Add me 
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory; 

	D3D12_CPU_DESCRIPTOR_HANDLE  m_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
	bool m_SwapChainOccluded = false;
	HANDLE m_hSwapChainWaitableObject = nullptr;
	HANDLE m_fenceEvent = nullptr;
	UINT64 m_fenceLastSignaledValue = 0;
	FrameContext m_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
	UINT m_frameIndex = 0;
	FrameContext* m_frameCtx;
	int m_Width;
	int m_Height;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	void FlushCommandQueue();
};