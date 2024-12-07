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

template<class T>
class VertexBuffer
{
private:
	VertexBuffer(const VertexBuffer<T>& rhs);
private:
	ComPtr<ID3D11Buffer> m_pBuffer;
	std::unique_ptr<UINT> m_pStride;
	UINT m_BufferSize;
public:
	VertexBuffer()
	{
	}

	ID3D11Buffer* Get() const noexcept
	{
		return m_pBuffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const noexcept
	{
		return m_pBuffer.GetAddressOf();
	}

	UINT BufferSize() const noexcept
	{
		return m_BufferSize;
	}

	const UINT Stride() const noexcept
	{
		return *m_pStride.get();
	}

	const UINT* StridePtr() const noexcept
	{
		return m_pStride.get();
	}

	HRESULT Initialize(ID3D11Device* pDevice, T* data, UINT numVertices)
	{
		if (m_pBuffer.Get() != nullptr)
		{
			m_pBuffer.Reset();
		}
		m_BufferSize = numVertices;
		if (m_pStride.get() == nullptr)
		{
			m_pStride = std::make_unique<UINT>(sizeof(T));
		}		

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(T) * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = data;

		HRESULT hr = pDevice->CreateBuffer(&bd, &sd, m_pBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create vertex buffer");
			return hr;
		}
		return hr;
	}
};