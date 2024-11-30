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
#include "GenException.h"
#include <sstream>
#include <vector>
#include <d3d11.h>
#include <GEngine/Graphics/D3D11/DX11Core.h>
#include "GContext.h"

#pragma comment(lib, "dxguid.lib")

namespace Genesis
{
	DxgiInfoManager::DxgiInfoManager()
	{
		// define function signature of DXGIGetDebugInterface
		typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

		// load the dll that contains the function DXGIGetDebugInterface
		const auto hModDxgiDebug = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hModDxgiDebug == nullptr)
		{
			throw GHWND_LAST_EXCEPT();
		}

		// get address of DXGIGetDebugInterface in dll
		const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
			reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
			);
		if (DxgiGetDebugInterface == nullptr)
		{
			throw GHWND_LAST_EXCEPT();
		}

		HRESULT hr;
		GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue));
	}

	void DxgiInfoManager::Set() noexcept
	{
		// set the index (next) so that the next all to GetMessages()
		// will only get errors generated after this call
		next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	}

	std::vector<std::string> DxgiInfoManager::GetMessages() const
	{
		std::vector<std::string> messages;
		const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
		for (auto i = next; i < end; i++)
		{
			HRESULT hr;
			SIZE_T messageLength;
			// get the size of message i in bytes
			GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
			// allocate memory for message
			auto bytes = std::make_unique<byte[]>(messageLength);
			auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
			// get the message and push its description into the vector
			GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
			messages.emplace_back(pMessage->pDescription);
		}
		return messages;
	}

	GenException::GenException(int line, const char* file) noexcept
		:
		line(line),
		file(file)
	{
	}
	const char* GenException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
			<< GetOriginString();
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}
	const char* GenException::GetType() const noexcept
	{
		return "Genesis Exception";
	}
	int GenException::GetLine() const noexcept
	{
		return line;
	}
	const std::string& GenException::GetFile() const noexcept
	{
		return file;
	}
	std::string GenException::GetOriginString() const noexcept
	{
		std::ostringstream oss;
		oss << "[File] " << file << std::endl
			<< "[Line] " << line;
		return oss.str();
	}

	// Window Exception Stuff
	std::string Exception::TranslateErrorCode(HRESULT hr) noexcept
	{
		char* pMsgBuf = nullptr;
		// windows will allocate memory for err string and make our pointer point to it
		const DWORD nMsgLen = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
		);
		// 0 string length returned indicates a failure
		if (nMsgLen == 0)
		{
			return "Unidentified error code";
		}
		// copy error string from windows-allocated buffer to std::string
		std::string errorString = pMsgBuf;
		// free windows buffer
		LocalFree(pMsgBuf);
		return errorString;
	}


	HrException::HrException(int line, const char* file, HRESULT hr) noexcept
		:
		Exception(line, file),
		hr(hr)
	{
	}

	const char* HrException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
			<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
			<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
			<< "[Description] " << GetErrorDescription() << std::endl
			<< GetOriginString();
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const char* HrException::GetType() const noexcept
	{
		return "Genesis Window Exception";
	}

	HRESULT HrException::GetErrorCode() const noexcept
	{
		return hr;
	}

	std::string HrException::GetErrorDescription() const noexcept
	{
		return Exception::TranslateErrorCode(hr);
	}


	const char* NoGfxException::GetType() const noexcept
	{
		return "Genesis Window Exception [No Graphics]";
	}

	GFXException::GFXException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
		:
		Exception(line, file),
		hr(hr)
	{
		// join all info messages with newlines into single string
		for (const auto& m : infoMsgs)
		{
			info += m;
			info.push_back('\n');
		}
		// remove final newline if exists
		if (!info.empty())
		{
			info.pop_back();
		}
	}

	const char* GFXException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
			<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
			<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
			<< "[Description] " << GetErrorDescription() << std::endl;
		if (!info.empty())
		{
			oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
		}
		oss << GetOriginString();
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const char* GFXException::GetType() const noexcept
	{
		return "Genesis Graphics Exception";
	}

	HRESULT GFXException::GetErrorCode() const noexcept
	{
		return hr;
	}

	std::string GFXException::GetErrorDescription() const noexcept
	{
		//HRESULT _hr = GContext::Get().GetGraphics()->GetDevice()->GetDeviceRemovedReason();
		return Exception::TranslateErrorCode(hr);
	}

	std::string GFXException::GetErrorInfo() const noexcept
	{
		return info;
	}

	const char* DeviceRemovedException::GetType() const noexcept
	{
		return "Genesis Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
	}
	InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
		:
		Exception(line, file)
	{
		// join all info messages with newlines into single string
		for (const auto& m : infoMsgs)
		{
			info += m;
			info.push_back('\n');
		}
		// remove final newline if exists
		if (!info.empty())
		{
			info.pop_back();
		}
	}


	const char* InfoException::what() const noexcept
	{
		std::ostringstream oss;
		oss << GetType() << std::endl
			<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
		oss << GetOriginString();
		whatBuffer = oss.str();
		return whatBuffer.c_str();
	}

	const char* InfoException::GetType() const noexcept
	{
		return "Genesis Graphics Info Exception";
	}

	std::string InfoException::GetErrorInfo() const noexcept
	{
		return info;
	}

} // namespace Genesis
