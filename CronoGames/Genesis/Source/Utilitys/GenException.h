/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "./Windows/GenWin.h"
#include <exception>
#include <string>
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>

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

class HRException : public GenException
{
public:
	HRException(int line, const char* file, HRESULT hr) noexcept;
	HRException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorString() const noexcept;
	std::string GetErrorDescription() const noexcept;
	std::string GetErrorInfo() const noexcept;
private:
	HRESULT hr;
	std::string info;
};

class DeviceRemovedException : public HRException
{
	using HRException::HRException;
public:
	const char* GetType() const noexcept override;
private:
	std::string reason;
};

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

// error exception helper macro
#define GENHR_EXCEPT( hr ) HRException( __LINE__,__FILE__,hr )
#define GENHR_LAST_EXCEPT() HRException( __LINE__,__FILE__,GetLastError() )

// gfx
#define GFX_EXCEPT_NOINFO(hr) HRException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throwHRException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) HRException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) HRException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(hr) HRException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) HRException( __LINE__,__FILE__,(hr) )
#endif
