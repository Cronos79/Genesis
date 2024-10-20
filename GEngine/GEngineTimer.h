#pragma once
#include <chrono>

class GEngineTimer
{
public:
	GEngineTimer() noexcept;
	float Mark() noexcept;
	float Peek() const noexcept;
private:
	std::chrono::steady_clock::time_point m_Last;
};