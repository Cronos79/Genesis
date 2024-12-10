#pragma once
#include "GEngine/Graphics/GraphicIncludes.h"

namespace Genesis
{
	class Graphics12
	{
	public:
		Graphics12(HWND hWnd, int32 width, int32 height);
		~Graphics12();
		void BeginFrame(float deltaTime);
		void EndFrame(float deltaTime);
		void Shutdown();
	private:
		void SetupDebugLayer();
		bool CreateDevice();
		bool CreateCommandQueue();
		bool CreateSwapChain(HWND hWnd, int32 width, int32 height);	
		bool CreateDescriptorHeaps();
		bool CreateRenderTargetViews();
		bool CreateCommandObjects();
		bool CreateFence();
		void CheckFeatureSupport();
		void WaitForPreviousFrame();
		void ClearFrame();
		ComPtr<IDXGIAdapter4> GetHardwareAdapter(IDXGIFactory4* factory);
	private:
		bool m_b4xMSAAEnabled = false;
		bool m_bRayTracingSupported = false;
		UINT m_CurrentBackBufferIndex = 0;

		static const uint32 m_NumBuffers = 2;

		ComPtr<IDXGIFactory4> m_pDxgiFactory;
		ComPtr<ID3D12Device14> m_pDevice = nullptr;
		ComPtr<IDXGISwapChain4> m_pSwapChain = nullptr;
		ComPtr<ID3D12CommandQueue> m_pCommandQueue = nullptr;
		ComPtr<ID3D12DescriptorHeap> m_pSrvDescHeap = nullptr;
		UINT m_SrvDescriptorSize = 0;
		ComPtr<ID3D12DescriptorHeap> m_pRtvDescHeap = nullptr;
		UINT m_RtvDescriptorSize = 0;
		ComPtr<ID3D12Resource> m_pRenderTargets[m_NumBuffers];
		ComPtr<ID3D12CommandAllocator> m_pCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_pCommandList = nullptr;
		
		ComPtr<ID3D12Fence> m_pFence = nullptr;
		HANDLE m_hFenceEvent = nullptr;
		uint64 m_FenceValue = 0;
		
		ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr;
		ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;
	};
}