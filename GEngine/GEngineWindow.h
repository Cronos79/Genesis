#pragma once
#include "Windows.h"
//#include "WinInclude.h"
#include "Keyboard.h"
#include "Mouse.h"

class GEngineWindow
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
		static constexpr const char* wndClassName = "BBEngineWindow";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	GEngineWindow(int width, int height, const char* name);
	~GEngineWindow();
	GEngineWindow(const GEngineWindow&) = delete;
	GEngineWindow& operator=(const GEngineWindow&) = delete;
	void SetTitle(const std::string& title);
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	static std::optional<int> ProcessMessages() noexcept;

	HWND GetHWND();
	HINSTANCE GetHInst();
	inline int GetWidth() { return m_Width; }
	inline int GetHeight() { return m_Height; }
private:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard m_Kbd;
	Mouse m_Mouse;
private:
	bool m_CursorEnabled = true;
	int m_Width;
	int m_Height;
	HWND m_hWnd;
	HINSTANCE m_HInst;
	std::vector<BYTE> m_RawBuffer;
	std::string m_CommandLine;
};
