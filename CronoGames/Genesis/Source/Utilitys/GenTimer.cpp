/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "GenTimer.h"
using namespace std::chrono;

GenTimer::GenTimer()
{
	last = steady_clock::now();
}

float GenTimer::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float GenTimer::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}
