#pragma once
#include "WinInclude.h"
#include <wrl.h>

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

constexpr auto SWAP_BUFFER_COUNT = 2;

class GEngineD3D12
{
public:
	GEngineD3D12(int32_t width, int32_t height, HINSTANCE hInstance, HWND hWnd);
	GEngineD3D12(const GEngineD3D12& rhs) = delete;
	GEngineD3D12& operator=(const GEngineD3D12& rhs) = delete;
	~GEngineD3D12();

public:
	inline Microsoft::WRL::ComPtr<ID3D12Device14>& GetDevice() { return m_Device; };
	inline Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCmdQueue() { return m_CmdQueue; };
	inline Microsoft::WRL::ComPtr<IDXGIFactory7>& GetDXGIFactory() { return m_dxgiFactory; };
	inline Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> GetCmdList() { return m_CmdList; }
	inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSRVHeap() { return m_srvDescHeap; };

	inline void SetShouldResize(bool shouldResize) { m_ShouldResize = shouldResize; };
	inline int32_t GetWidth() const { return m_Width; };
	inline int32_t GetHeight() const { return m_Height; };
	inline void SetWindowSize(int32_t width, int32_t height) { m_Width = width; m_Height = height; SetShouldResize(true); }

	bool InIt();
	void BeginFrame(float dt);
	void EndFrame(float dt);
	void Shutdown();
	void ExecuteCommandList();
	ID3D12GraphicsCommandList10* InitCommandList();
private:	
	void Flush(size_t count);
	void SignalAndWait();	
	void ResizeSwapChain();
	bool GetBuffers();
	void ReleaseBuffers();
	void SetDarkThemeColors();
	
	
private:
	Microsoft::WRL::ComPtr<ID3D12Device14> m_Device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence;
	UINT64 M_FenceValue = 0;
	HANDLE m_FenceEvent = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> m_CmdList;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
	Microsoft::WRL::ComPtr<IDXGIFactory7> m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Resource2> m_Buffers[SWAP_BUFFER_COUNT];
	size_t m_CurrentBufferIndex = 0;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvDescHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandles[SWAP_BUFFER_COUNT];

	bool m_ShouldResize = false;
	bool m_SwapChainOccluded = false;
	int32_t m_Width;
	int32_t m_Height;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
};