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
#include "AdapterReader.h"
#include "../Core/GContext.h"
#include "GEngine/Logger/Log.h"

namespace Genesis
{
	std::vector<AdapterData> AdapterReader::m_Adapters;

	Genesis::AdapterData::AdapterData(IDXGIAdapter1* pAdapter)
	{
		m_pAdapter = pAdapter;
		HRESULT hr = m_pAdapter->GetDesc1(&m_Desc);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to get adapter description");
			exit(-1);
		}
	}

	std::vector<AdapterData> AdapterReader::GetAdapters()
	{
		if (!m_Adapters.empty())
			return m_Adapters;
		
		Microsoft::WRL::ComPtr<IDXGIFactory1> pFactory;

		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create DXGIFactory");
			exit(-1);
		}

		IDXGIAdapter1* pAdapter = nullptr;
		UINT adapterIndex = 0;
		while (SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &pAdapter)))
		{
			m_Adapters.emplace_back(pAdapter);
			adapterIndex++;
		}
		return m_Adapters;
	}
}

