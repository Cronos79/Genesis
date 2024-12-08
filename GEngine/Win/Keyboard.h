/******************************************************************************************
*	CronoGames Game Engine																  *
*	Copyright � 2024 CronoGames <http://www.cronogames.net>								  *
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
#include "GEngine/Core/GEnginePCH.h"

namespace Genesis
{
	class Keyboard
	{
		friend class Window;
	public:
		class Event
		{
		public:
			enum class Type
			{
				Press,
				Release,
			};
		private:
			Type type;
			unsigned char code;
		public:
			Event(Type type, unsigned char code) noexcept
				:
				type(type),
				code(code)
			{
			}
			bool IsPress() const noexcept
			{
				return type == Type::Press;
			}
			bool IsRelease() const noexcept
			{
				return type == Type::Release;
			}
			unsigned char GetCode() const noexcept
			{
				return code;
			}
		};
	public:
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;
		// key event stuff
		bool KeyIsPressed(unsigned char keycode) const noexcept;
		std::optional<Event> ReadKey() noexcept;
		bool KeyIsEmpty() const noexcept;
		void FlushKey() noexcept;
		// char event stuff
		std::optional<char> ReadChar() noexcept;
		bool CharIsEmpty() const noexcept;
		void FlushChar() noexcept;
		void Flush() noexcept;
		// autorepeat control
		void EnableAutorepeat() noexcept;
		void DisableAutorepeat() noexcept;
		bool AutorepeatIsEnabled() const noexcept;
	private:
		void OnKeyPressed(unsigned char keycode) noexcept;
		void OnKeyReleased(unsigned char keycode) noexcept;
		void OnChar(char character) noexcept;
		void ClearState() noexcept;
		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer) noexcept;
	private:
		static constexpr unsigned int nKeys = 256u;
		static constexpr unsigned int bufferSize = 16u;
		bool autorepeatEnabled = false;
		std::bitset<nKeys> keystates;
		std::queue<Event> keybuffer;
		std::queue<char> charbuffer;
	};
}
