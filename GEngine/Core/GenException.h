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
#include "GEngine/Win/WinInclude.h"
#include "GEngine/Core/CommonHeaders.h"
#include <exception>
#include <string>
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>

namespace Genesis
{
	class DxgiInfoManager
	{
	public:
		DxgiInfoManager();
		~DxgiInfoManager() = default;
		DxgiInfoManager(const DxgiInfoManager&) = delete;
		DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
		void Set() noexcept;
		std::vector<std::string> GetMessages() const;
	private:
		unsigned long long next = 0u;
		Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
	};

	class GenException : public std::exception
	{
	public:
		GenException(int line, const char* file) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		int GetLine() const noexcept;
		const std::string& GetFile() const noexcept;
		std::string GetOriginString() const noexcept;
	private:
		int line;
		std::string file;
	protected:
		mutable std::string whatBuffer;
	};

	class Exception : public GenException
	{
		using GenException::GenException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};
	class GFXException : public Exception
	{
	public:
		GFXException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public GFXException
	{
		using GFXException::GFXException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
}

// Windows Exception Macro
#define GHWND_EXCEPT( hr ) Genesis::HrException( __LINE__,__FILE__,(hr) )
#define GHWND_LAST_EXCEPT() Genesis::HrException( __LINE__,__FILE__,GetLastError() )
#define GHWND_NOGFX_EXCEPT() Genesis::NoGfxException( __LINE__,__FILE__ )

// DirectX Exception Macro
#define GFX_EXCEPT_NOINFO(hr) Genesis::GFXException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Genesis::GFXException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Genesis::GFXException( __LINE__,__FILE__,(hr),infoManager->GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager->Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Genesis::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager->GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager->Set(); (call); {auto v = infoManager->GetMessages(); if(!v.empty()) {throw Genesis::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Genesis::GFXException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Genesis::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager(Graphics& gfx) must exist
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager* infoManager = GetInfoManager((gfx))
#endif

#ifdef NDEBUG
#define INFOMAN_NOHR(gfx)
#else
#define INFOMAN_NOHR(gfx) DxgiInfoManager* infoManager = GetInfoManager((gfx))
#endif
