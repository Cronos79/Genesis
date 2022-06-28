/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "./Windows/GenWin.h"
#include <exception>
#include <string>

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
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorString() const noexcept;
private:
	HRESULT hr;
};

// error exception helper macro
#define GENHR_EXCEPT( hr ) HRException( __LINE__,__FILE__,hr )
#define GENHR_LAST_EXCEPT() HRException( __LINE__,__FILE__,GetLastError() )
