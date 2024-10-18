#include "genginepch.h"
#include "GEngineTimer.h"

using namespace std::chrono;

GEngineTimer::GEngineTimer() noexcept
{
	m_Last = steady_clock::now();
}

float GEngineTimer::Mark() noexcept
{
	const auto old = m_Last;
	m_Last = steady_clock::now();
	const duration<float> frameTime = m_Last - old;
	return frameTime.count();
}

float GEngineTimer::Peek() const noexcept
{
	return duration<float>(steady_clock::now() - m_Last).count();
}
