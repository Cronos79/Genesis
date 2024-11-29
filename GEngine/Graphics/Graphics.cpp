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
#include "Graphics.h"

namespace Genesis
{
	Graphics::Graphics(GraphicsAPI api)
	{
		m_API = api;
		switch (m_API)
		{
		case GraphicsAPI::OpenGL:
			break;
		case GraphicsAPI::DirectX11:
			m_DX11Core = new DX11Core();
			break;
		case GraphicsAPI::DirectX12:
			break;
		case GraphicsAPI::Vulkan:
			break;
		default:
			break;
		}
	}
	Graphics::~Graphics()
	{
	}
	void Graphics::Init()
	{
		switch (m_API)
		{
		case GraphicsAPI::OpenGL:
			break;
		case GraphicsAPI::DirectX11:
			m_DX11Core->Init();
			break;
		case GraphicsAPI::DirectX12:
			break;
		case GraphicsAPI::Vulkan:
			break;
		default:
			break;
		}
	}
	void Graphics::Shutdown()
	{
		switch (m_API)
		{
		case GraphicsAPI::OpenGL:
			break;
		case GraphicsAPI::DirectX11:
			m_DX11Core->Shutdown();
			break;
		case GraphicsAPI::DirectX12:
			break;
		case GraphicsAPI::Vulkan:
			break;
		default:
			break;
		}
	}
	void Graphics::BeginFrame()
	{
		switch (m_API)
		{
		case GraphicsAPI::OpenGL:
			break;
		case GraphicsAPI::DirectX11:
			m_DX11Core->BeginFrame();
			break;
		case GraphicsAPI::DirectX12:
			break;
		case GraphicsAPI::Vulkan:
			break;
		default:
			break;
		}
	}
	void Graphics::EndFrame()
	{
		switch (m_API)
		{
		case GraphicsAPI::OpenGL:
			break;
		case GraphicsAPI::DirectX11:
			m_DX11Core->EndFrame();
			break;
		case GraphicsAPI::DirectX12:
			break;
		case GraphicsAPI::Vulkan:
			break;
		default:
			break;
		}
	}
} // namespace Genesis