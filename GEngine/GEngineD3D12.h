#pragma once
#include "WinInclude.h"
#include <wrl.h>

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#define SWAP_BUFFER_COUNT 2

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

	inline void SetShouldResize(bool shouldResize) { m_ShouldResize = shouldResize; };
	inline int32_t GetWidth() { return m_Width; };
	inline int32_t GetHeight() { return m_Height; };

	bool InIt();
	void BeginRender(float dt);
	void EndRender(float dt);
	void Shutdown();

private:
	ID3D12GraphicsCommandList10* InitCommandList();
	void ExecuteCommandList();	
	void Flush(size_t count);
	void SignalAndWait();	
	void ResizeSwapChain();

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

	bool m_ShouldResize = false;
	bool m_SwapChainOccluded = false;
	int32_t m_Width;
	int32_t m_Height;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
};