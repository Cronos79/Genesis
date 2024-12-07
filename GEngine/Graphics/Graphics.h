/******************************************************************************************
*	CronoGames Game Engine																  *
*	Copyright � 2024 CronoGames <http://www.cronogames.net>								  *
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
#include "GEngine/Graphics/GraphicsIncludes.h"
#include "D3D11/DX11Core.h"

enum class GraphicsAPI
{
	OpenGL,
	DirectX11,
	DirectX12,
	Vulkan
};

namespace Genesis
{
	class Graphics
	{
	public:
		Graphics(GraphicsAPI api);
		~Graphics();	
		void Init();
		void Shutdown();
		void BeginFrame(float deltaTime);
		void EndFrame(float deltaTime);

		inline DX11Core* GetDX11Core() const noexcept
		{
			return m_DX11Core;
		}

		inline ID3D11Device* GetDevice() const noexcept
		{
			return m_DX11Core->GetDevice();
		}

	private:
		GraphicsAPI m_API;
		DX11Core* m_DX11Core;
	};
} // namespace Genesis
