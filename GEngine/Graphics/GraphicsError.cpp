#include "GEngine/Core/GEnginePCH.h"
#include "GraphicsError.h"
#include <ranges>
#include <format>
#include "GEngine/Core/StringHelper.h"
#include "GEngine/Logger/Log.h"

namespace rn = std::ranges;
namespace vi = rn::views;


namespace Genesis
{

	std::wstring GetErrorDescription(HRESULT hr)
	{
		wchar_t* descriptionWinalloc = nullptr;
		const auto result = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPWSTR>(&descriptionWinalloc), 0, nullptr
		);

		std::wstring description;
		if (!result)
		{
			LOG_WARNING("Failed formatting windows error");
		}
		else
		{
			description = descriptionWinalloc;
			if (LocalFree(descriptionWinalloc))
			{
				LOG_WARNING("Failed freeing memory for windows error formatting");
			}
			if (description.ends_with(L"\r\n"))
			{
				description.resize(description.size() - 2);
			}
		}
		return description;
	}

	CheckerToken chk;

	HrGrabber::HrGrabber(unsigned int hr, std::source_location loc) noexcept
		:
		hr(hr),
		loc(loc)
	{
	}
	void operator>>(HrGrabber g, CheckerToken)
	{
		if (FAILED(g.hr))
		{
			// get error description as narrow string with crlf removed
			auto errorString = StringHelper::WideToString(GetErrorDescription(g.hr)) |
				vi::transform([](char c) {return c == '\n' ? ' ' : c; }) |
				vi::filter([](char c) {return c != '\r'; }) |
				rn::to<std::basic_string>();
			throw std::runtime_error{
				std::format("Graphics Error: {}\n   {}({})",
				errorString, g.loc.file_name(), g.loc.line())
			};
		}
	}
}