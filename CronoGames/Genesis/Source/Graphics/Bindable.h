/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "GenGraphics.h"

class Bindable
{
public:
	virtual void Bind(GenGraphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	static ID3D11DeviceContext* GetContext(GenGraphics& gfx) noexcept;
	static ID3D11Device* GetDevice(GenGraphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(GenGraphics& gfx) noexcept(IS_DEBUG);
};

