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
#pragma once
#include "GEngine/Core/CommonHeaders.h"

namespace Genesis
{
	class Log
	{
	public:
		static void Info(std::string message, int32 line, const char* file);
		static void Warning(std::string message, int32 line, const char* file);
		static void Error(std::string message, int32 line, const char* file);
	private:

		// Singleton
	public:
		inline static Log& Get()
		{
			static Log instance;
			return instance;
		}
		// Delete copy constructor and assignment operator
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;
	private:
		Log() = default;
		~Log() = default;
	};
}

#define LOG_INFO(message) Genesis::Log::Get().Info(message, __LINE__, __FILE__)
#define LOG_WARNING(message) Genesis::Log::Get().Warning(message, __LINE__, __FILE__)
#define LOG_ERROR(message) Genesis::Log::Get().Error(message, __LINE__, __FILE__)

