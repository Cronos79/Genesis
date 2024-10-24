#pragma once
#include "WinInclude.h"
#include <cstdlib>
#include <filesystem>
#include <string_view>
#include <fstream>

class GEngineShader
{
public:
	GEngineShader(std::string_view shaderName);
	~GEngineShader();

	inline const void* GetBuffer() const { return m_Data; }
	inline size_t GetSize() const { return m_Size; }

private:	
	void* m_Data = nullptr;
	size_t m_Size = 0;
};