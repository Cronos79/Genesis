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
#pragma once
#include "GEngine/Graphics/GraphicsIncludes.h"

namespace Genesis
{
	struct Vertex
	{
		Vertex()
		{
			position = { 0.0f, 0.0f, 0.0f };
		}
		Vertex(float x, float y, float z)
			: position(x, y, z)
		{
		}

		dx::XMFLOAT3 position{};
	};

	class VertexShader
	{
	public:
		bool Initialize(ComPtr<ID3D11Device>& pDevice, const std::wstring& shaderPath, const D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements);
		ID3D11VertexShader* GetShader() const noexcept;
		ID3D10Blob* GetBlob() const noexcept;
		ID3D11InputLayout* GetInputLayout() const noexcept;
	private:
		ComPtr<ID3D11VertexShader> m_pVertexShader = nullptr;
		ComPtr<ID3D10Blob> m_pBlob = nullptr;
		ComPtr<ID3D11InputLayout> m_pInputLayout;
	};

	class PixelShader
	{
	public:
		bool Initialize(ComPtr<ID3D11Device>& pDevice, const std::wstring& shaderPath);
		ID3D11PixelShader* GetShader() const noexcept;
		ID3D10Blob* GetBlob() const noexcept;
	private:
		ComPtr<ID3D11PixelShader> m_pPixelShader = nullptr;
		ComPtr<ID3D10Blob> m_pBlob = nullptr;
	};
}