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
	~GenGraphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
	DxgiInfoManager infoManager;
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};

