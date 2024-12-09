#pragma once
#include "GEngine/Win/WinInclude.h"
#include "GEngine/Graphics/GraphicIncludes.h"
#include "GEngine/Core/GEnginePCH.h"

namespace Genesis
{
	class Graphics
	{
	public:
		Graphics(HWND hWnd, int32 width, int32 height);
		~Graphics();

		void BeginFrame(float deltaTime);
		void EndFrame(float deltaTime);

	private:
		void ClearFrame();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;
	};
}
