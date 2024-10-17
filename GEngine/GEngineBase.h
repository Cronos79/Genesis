#pragma once

#if GE_DLL
#define GE_API __declspec(dllexport)
#else
#define GE_API __declspec(dllimport)
#endif