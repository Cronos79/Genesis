/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "./Windows/GenWin.h"
#include <d3d11.h>
#include "Utilitys/GenException.h"
#include <wrl.h>
#include <vector>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

class GenGraphics
{
	friend class Bindable;
public:
	GenGraphics(HWND hWnd, int wndWidth, int wndHeight);
	GenGraphics(const GenGraphics&) = delete;
	GenGraphics& operator=(const GenGraphics&) = delete;
	~GenGraphics() = default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
private:
private:
	DirectX::XMMATRIX projection;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};

