#pragma once
#include "GEngine/Core/GEnginePCH.h"
#include <source_location>

namespace Genesis
{
	std::wstring GetErrorDescription(HRESULT hr);
	struct CheckerToken {
	};
	extern CheckerToken chk;
	struct HrGrabber {
		HrGrabber(unsigned int hr, std::source_location = std::source_location::current()) noexcept;
		unsigned int hr;
		std::source_location loc;
	};
	void operator>>(HrGrabber, CheckerToken);
}

