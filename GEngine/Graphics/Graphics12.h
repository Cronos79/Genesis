/******************************************************************************************
*	CronoGames Game Engine																  *
*	Copyright © 2024 CronoGames <http://www.cronogames.net>								  *
*																						  *
*	This file is part of CronoGames Game Engine.										  *
*																						  *
*	CronoGames Game Engine is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The CronoGames Game Engine is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The CronoGames Game Engine.  If not, see <http://www.gnu.org/licenses/>.   *
******************************************************************************************/
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
		void ToggleVSync();
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
		bool InitImGui(HWND hWnd);
		void WaitForPreviousFrame();
		void ClearFrame(float deltaTime);
		void Flush();
		ComPtr<IDXGIAdapter4> GetHardwareAdapter(IDXGIFactory4* factory);
	private:
		bool m_b4xMSAAEnabled = false;
		bool m_bRayTracingSupported = false;
		UINT m_CurrentBackBufferIndex = 0;
		bool m_VSync = true;

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