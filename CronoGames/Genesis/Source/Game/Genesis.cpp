/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#include "Genesis.h"

Genesis::Genesis()
	:
	wnd(1920, 1080, "Genesis")
{

}

int Genesis::Start()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = GenWindow::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		Update();
	}
}

void Genesis::Update()
{
	const float dt = timer.Mark() * 1000;
	std::ostringstream oss;
	oss << "Time: " << std::setprecision(6) << std::fixed << dt << "s\n";
	OutputDebugString(oss.str().c_str());
}
