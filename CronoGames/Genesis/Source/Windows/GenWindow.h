/*
* Creator: Matt Brock
* Notice: (C) Copyright 2022 by CronoGames, Inc. All Rights Reserved.
*/
#pragma once
#include "GenWin.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "Keyboard.h"
#include "Mouse.h"
#include "./imgui/imgui_impl_win32.h"
#include "Graphics/GenGraphics.h"

class GenWindow
{
private:
	// singleton manages registration/cleanup of window class
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "GenesisGameWindowClass";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	GenWindow(int width, int height, const char* name);
	~GenWindow();
	GenWindow(const GenWindow&) = delete;
	GenWindow& operator=(const GenWindow&) = delete;

	void SetTitle(const std::string& title);
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	static std::optional<int> ProcessMessages();
	GenGraphics& Gfx();
private:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Keyboard kbd;
	Mouse mouse;
private:
	bool cursorEnabled = true;
	int width;
	int height;
	HWND hWnd;
	std::vector<BYTE> rawBuffer;
	std::string commandLine;
	std::unique_ptr<GenGraphics> pGfx;
};