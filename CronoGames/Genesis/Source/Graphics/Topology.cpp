/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "Topology.h"

namespace Bind
{
	Topology::Topology(GenGraphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
		:
		type(type)
	{}

	void Topology::Bind(GenGraphics& gfx) noexcept
	{
		GetContext(gfx)->IASetPrimitiveTopology(type);
	}
}