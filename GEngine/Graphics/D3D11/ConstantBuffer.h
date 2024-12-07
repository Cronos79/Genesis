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
#include "GEngine/Graphics/D3D11/ConstantBufferTypes.h"

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);
private:
	ComPtr<ID3D11Buffer> m_pBuffer;
	ID3D11DeviceContext* m_pDeviceContext;
public:
	ConstantBuffer() { }

	T data;

	ID3D11Buffer* Get() const noexcept
	{
		return m_pBuffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf() const noexcept
	{
		return m_pBuffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		if (m_pBuffer.Get() != nullptr)
		{
			m_pBuffer.Reset();
		}
		m_pDeviceContext = pDeviceContext;

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - sizeof(T) % 16));
		cbd.StructureByteStride = 0u;

		HRESULT hr = pDevice->CreateBuffer(&cbd, nullptr, m_pBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create constant buffer");
			return hr;
		}
		return hr;
	}

	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		HRESULT hr = m_pDeviceContext->Map(m_pBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to map constant buffer");
			return false;
		}
		memcpy(msr.pData, &data, sizeof(data));
		m_pDeviceContext->Unmap(m_pBuffer.Get(), 0u);
		return true;
	}
};