/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(GenGraphics& gfx, const std::wstring& path);
	void Bind(GenGraphics& gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};

