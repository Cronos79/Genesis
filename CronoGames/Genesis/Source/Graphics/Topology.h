/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "Bindable.h"

namespace Bind
{
	class Topology : public Bindable
	{
	public:
		Topology(GenGraphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(GenGraphics& gfx) noexcept override;
	protected:
		D3D11_PRIMITIVE_TOPOLOGY type;
	};
}

