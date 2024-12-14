#pragma once 
#include <string> 
#include "ChilWin.h" 
#include <GEngine/src/spa/Dimensions.h> 
#include <GEngine/src/spa/Rect.h> 

namespace chil::win
{
	std::wstring GetErrorDescription(HRESULT hr);
	RECT ToWinRect(const spa::RectI&);
	spa::RectI ToSpaRect(const RECT&);
	spa::DimensionsI ClientToWindowDimensions(const spa::DimensionsI& dims, DWORD styles);
}