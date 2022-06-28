/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "./Windows/GenWin.h"
#include <d3d11.h>
#include "Utilitys/GenException.h"

class GenGraphics
{
public:
	GenGraphics(HWND hWnd);
	GenGraphics(const GenGraphics&) = delete;
	GenGraphics& operator=(const GenGraphics&) = delete;
	~GenGraphics() = default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
	DxgiInfoManager infoManager;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};

