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
#include "Shaders.h"

namespace Genesis
{

	bool VertexShader::Initialize(ComPtr<ID3D11Device>& pDevice, const std::wstring& shaderPath, const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
	{
		HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_pBlob.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to read shader file");
			return false;
		}

		hr = pDevice->CreateVertexShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create vertex shader");
			return false;
		}

		hr = pDevice->CreateInputLayout(
			layoutDesc, numElements,
			m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(),
			m_pInputLayout.GetAddressOf()
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create input layout");
			return false;
		}

		return true;
	}

	ID3D11VertexShader* VertexShader::GetShader() const noexcept
	{
		return m_pVertexShader.Get();
	}

	ID3D10Blob* VertexShader::GetBlob() const noexcept
	{
		return m_pBlob.Get();
	}
	ID3D11InputLayout* VertexShader::GetInputLayout() const noexcept
	{
		return m_pInputLayout.Get();
	}
	bool PixelShader::Initialize(ComPtr<ID3D11Device>& pDevice, const std::wstring& shaderPath)
	{
		HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), m_pBlob.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to read shader file");
			return false;
		}

		hr = pDevice->CreatePixelShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create pixel shader");
			return false;
		}

		return true;
	}
	ID3D11PixelShader* PixelShader::GetShader() const noexcept
	{
		return m_pPixelShader.Get();
	}
	ID3D10Blob* PixelShader::GetBlob() const noexcept
	{
		return m_pBlob.Get();
	}
}