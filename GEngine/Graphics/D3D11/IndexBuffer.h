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

class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);
private:
	ComPtr<ID3D11Buffer> m_pBuffer;
	UINT m_BufferSize;
public:
	IndexBuffer()
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

	HRESULT Initialize(ID3D11Device* pDevice, const DWORD* data, UINT numIndices)
	{
		if (m_pBuffer.Get() != nullptr)
		{
			m_pBuffer.Reset();
		}
		m_BufferSize = numIndices;
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.ByteWidth = sizeof(DWORD) * numIndices;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;

		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = data;

		HRESULT hr = pDevice->CreateBuffer(&ibd, &isd, m_pBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create index buffer");
		}
		return hr;
	}
};