#pragma once
#include "WinInclude.h"
#include <exception>
#include <string>


	class GEngineException : public std::exception
	{
	public:
		GEngineException(int line, const char* file) noexcept;
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

	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HrException : public GEngineException
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
	class NoGfxException : public GEngineException
	{
	public:
		using GEngineException::GEngineException;
		const char* GetType() const noexcept override;
	};


#define CHWND_EXCEPT( hr ) HrException( __LINE__,__FILE__,(hr) )
#define CHWND_LAST_EXCEPT() HrException( __LINE__,__FILE__,GetLastError() )
#define CHWND_NOGFX_EXCEPT() NoGfxException( __LINE__,__FILE__ )