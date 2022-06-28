/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "GenException.h"
#include <sstream>

GenException::GenException(int line, const char* file) noexcept
	: line(line), file(file)
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
	return "Gen Exception";
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

// HR Exception
HRException::HRException(int line, const char* file, HRESULT hr) noexcept
	: GenException(line, file),
	hr(hr)
{

}

const char* HRException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* HRException::GetType() const noexcept
{
	return "Gen HR Exception";
}

std::string HRException::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessage(
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

HRESULT HRException::GetErrorCode() const noexcept
{
	return hr;
}

std::string HRException::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}
