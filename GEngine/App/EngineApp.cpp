/******************************************************************************************
*	CronoGames Game Engine																  *
*	Copyright © 2024 CronoGames <http://www.cronogames.net>								  *
*																						  *
*	This file is part of CronoGames Game Engine.										  *
*																						  *
*	CronoGames Game Engine is free software: you can redistribute it and/or modify		  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The CronoGames Game Engine is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The CronoGames Game Engine.  If not, see <http://www.gnu.org/licenses/>.   *
******************************************************************************************/
#include "GEngine/Core/GEnginePCH.h"
#include "EngineApp.h"
#include "Gengine/Core/GContext.h"
#include "GEngine/Win/Window.h"

namespace Genesis
{
	EngineApp::EngineApp()
	{
	}

	int EngineApp::Run()
	{
		Init();
		float deltaTime = 0.0f;
		while (GContext::Get().IsRunning())
		{
			if (const auto ecode = GContext::Get().GetWindow()->ProcessMessages())
			{
				// if return optional has value, means we're quitting so return exit code
				Shutdown();
				GContext::Get().Shutdown();
				return *ecode;
			}
			GContext::Get().GetGraphics12()->BeginFrame(deltaTime);
			HandleInput(deltaTime);
			Update(deltaTime);
			GContext::Get().GetGraphics12()->EndFrame(deltaTime);
		}
		Shutdown();
		GContext::Get().Shutdown();
		return -1;
	}
}