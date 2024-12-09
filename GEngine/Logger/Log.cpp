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
#include "Log.h"
#include "GEngine/Win/WinInclude.h"

namespace Genesis
{

	void Log::Info(std::string message, int32 line, const char* file)
	{
		std::string linestr = std::to_string(line);
		message = "INFO: " + message + "\n" + "INFO: " + file + "\n" + "INFO: Line: (" + linestr + ")\n";
		OutputDebugStringA(message.c_str());
	}

	void Log::Warning(std::string message, int32 line, const char* file)
	{
		std::string linestr = std::to_string(line);
		message = "WARNING: " + message + "\n" + "WARNING: " + file + "\n" + "WARNING: Line: (" + linestr + ")\n";
		OutputDebugStringA(message.c_str());
	}

	void Log::Error(std::string message, int32 line, const char* file)
	{
		std::string linestr = std::to_string(line);
		message = "ERROR: " + message + "\n" + "ERROR: " + file + "\n" + "ERROR: Line: (" + linestr + ")\n";
		OutputDebugStringA(message.c_str());
	}

}