#include "genginepch.h"
#include "GEngineWindow.h"
#include "GEngineException.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "GEngineContext.h"
#include "GEngineLog.h"


// Window Class Stuff
GEngineWindow::WindowClass GEngineWindow::WindowClass::wndClass;

GEngineWindow::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	RegisterClassEx(&wc);
}

GEngineWindow::WindowClass::~WindowClass()
{
	UnregisterClassA(wndClassName, GetInstance());
}

const char* GEngineWindow::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE GEngineWindow::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

GEngineWindow::GEngineWindow(int width, int height, const char* name)
	:
	m_Width(width),
	m_Height(height),
	m_WinName(name)
{
	
}

GEngineWindow::~GEngineWindow()
{
	
}

void GEngineWindow::Init()
{
	BB_TRACE("Windows starting up");
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = m_Width + wr.left;
	wr.top = 100;
	wr.bottom = m_Height + wr.top;
	if (AdjustWindowRect(&wr, WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, FALSE) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
	// create window & get hWnd
		// Get screen dimensions
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Calculate position to center the window
	int xPos = (screenWidth - (wr.right - wr.left)) / 2;
	int yPos = (screenHeight - (wr.bottom - wr.top)) / 2;

	m_hWnd = CreateWindowEx(
		0/*WS_EX_LAYERED*/,
		WindowClass::GetName(), m_WinName,
		WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		xPos, yPos, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	// check for error
	if (m_hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	// newly created windows start off as hidden
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	// register mouse raw input device
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

void GEngineWindow::ShutDown()
{
	BB_TRACE("Windows shutting down");
	DestroyWindow(m_hWnd);
}

void GEngineWindow::SetTitle(const std::string& title)
{
	if (SetWindowText(m_hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

void GEngineWindow::EnableCursor() noexcept
{
	m_CursorEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void GEngineWindow::DisableCursor() noexcept
{
	m_CursorEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

bool GEngineWindow::CursorEnabled() const noexcept
{
	return m_CursorEnabled;
}

std::optional<int> GEngineWindow::ProcessMessages() noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return (int)msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

void GEngineWindow::SetFullScreen(bool isFullScreen)
{
	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
	if (isFullScreen)
	{
		style = WS_POPUP | WS_VISIBLE;
		exStyle = WS_EX_APPWINDOW;
	}

	SetWindowLongW(m_hWnd, GWL_STYLE, style);
	SetWindowLongW(m_hWnd, GWL_EXSTYLE, exStyle);

	if (isFullScreen)
	{
		HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info{};
		info.cbSize = sizeof(info);
		if (GetMonitorInfoW(monitor, &info))
		{
			RECT wr;
			wr.left = info.rcMonitor.left;
			wr.right = info.rcMonitor.right - info.rcMonitor.left;
			wr.top = info.rcMonitor.top;
			wr.bottom = info.rcMonitor.bottom - info.rcMonitor.top;
			if (AdjustWindowRect(&wr, exStyle, FALSE) == 0)
			{
				throw CHWND_LAST_EXCEPT();
			}
			SetWindowPos(m_hWnd, nullptr,
				wr.left,
				wr.top,
				wr.right - wr.left,
				wr.bottom - wr.top, SWP_NOZORDER);

			GEngineContext::GetInstance().GetGFX()->SetWindowSize(wr.right - wr.left, wr.bottom - wr.top);
		}
	}

	m_IsFullScreen = isFullScreen;
}

HWND GEngineWindow::GetHWND()
{
	if (m_hWnd != nullptr)
	{
		return m_hWnd;
	}
	return nullptr;
}

HINSTANCE GEngineWindow::GetHInst()
{
	return WindowClass::GetInstance();
}

void GEngineWindow::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void GEngineWindow::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

void GEngineWindow::ShowCursor() noexcept
{
	while (::ShowCursor(TRUE) < 0);
}

void GEngineWindow::HideCursor() noexcept
{
	while (::ShowCursor(FALSE) >= 0);
}

void GEngineWindow::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void GEngineWindow::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

LRESULT CALLBACK GEngineWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		GEngineWindow* const pWnd = static_cast<GEngineWindow*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&GEngineWindow::HandleMsgRedirect));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK GEngineWindow::HandleMsgRedirect(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window instance
	GEngineWindow* const pWnd = reinterpret_cast<GEngineWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT GEngineWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
		/*ImGuiIO imio;
		if (m_imguIisInit)
		{
			imio = ImGui::GetIO();
		}*/

	switch (msg)
	{
	case WM_SIZE:
		if (GEngineContext::GetInstance().GetGFX() != nullptr)
		{
			if (lParam && (HIWORD(lParam) != GEngineContext::GetInstance().GetGFX()->GetHeight() || LOWORD(lParam) != GEngineContext::GetInstance().GetGFX()->GetWidth()))
			{
				GEngineContext::GetInstance().GetGFX()->SetShouldResize(true);
			}			
		}		
		break;
		// we don't want the DefProc to handle this message because
		// we want our destructor to destroy the window, so return 0 instead of break
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// clear keystate when window loses focus to prevent input getting "stuck"
	case WM_KILLFOCUS:
		m_Kbd.ClearState();
		break;
	case WM_ACTIVATE:
		// confine/free cursor on window to foreground/background if cursor disabled
		if (!m_CursorEnabled)
		{
			if (wParam & WA_ACTIVE)
			{
				ConfineCursor();
				HideCursor();
			}
			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
		break;

		/*********** KEYBOARD MESSAGES ***********/
	case WM_KEYDOWN:
		// syskey commands need to be handled to track ALT key (VK_MENU) and F10
	case WM_SYSKEYDOWN:
		// stifle this keyboard message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureKeyboard)
		{
			break;
		}*/
		if (!(lParam & 0x40000000) || m_Kbd.AutorepeatIsEnabled()) // filter autorepeat
		{
			m_Kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// stifle this keyboard message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureKeyboard)
		{
			break;
		}*/
		m_Kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		// stifle this keyboard message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureKeyboard)
		{
			break;
		}*/
		m_Kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/*********** END KEYBOARD MESSAGES ***********/

		/************* MOUSE MESSAGES ****************/
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// cursorless exclusive gets first dibs
		if (!m_CursorEnabled)
		{
			if (!m_Mouse.IsInWindow())
			{
				SetCapture(hWnd);
				m_Mouse.OnMouseEnter();
				HideCursor();
			}
			break;
		}
		// stifle this mouse message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureMouse)
		{
			break;
		}*/
		// in client region -> log move, and log enter + capture mouse (if not previously in window)
		if (pt.x >= 0 && pt.x < m_Width && pt.y >= 0 && pt.y < m_Height)
		{
			m_Mouse.OnMouseMove(pt.x, pt.y);
			if (!m_Mouse.IsInWindow())
			{
				SetCapture(hWnd);
				m_Mouse.OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				m_Mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
			else
			{
				ReleaseCapture();
				m_Mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SetForegroundWindow(hWnd);
		if (!m_CursorEnabled)
		{
			ConfineCursor();
			HideCursor();
		}
		// stifle this mouse message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureMouse)
		{
			break;
		}*/
		const POINTS pt = MAKEPOINTS(lParam);
		m_Mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// stifle this mouse message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureMouse)
		{
			break;
		}*/
		const POINTS pt = MAKEPOINTS(lParam);
		m_Mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureMouse)
		{
			break;
		}*/
		const POINTS pt = MAKEPOINTS(lParam);
		m_Mouse.OnLeftReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= m_Width || pt.y < 0 || pt.y >= m_Height)
		{
			ReleaseCapture();
			m_Mouse.OnMouseLeave();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureMouse)
		{
			break;
		}*/
		const POINTS pt = MAKEPOINTS(lParam);
		m_Mouse.OnRightReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= m_Width || pt.y < 0 || pt.y >= m_Height)
		{
			ReleaseCapture();
			m_Mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// stifle this mouse message if imgui wants to capture
	/*	if (m_imguIisInit && imio.WantCaptureMouse)
		{
			break;
		}*/
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		m_Mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	/************** END MOUSE MESSAGES **************/

	/************** RAW MOUSE MESSAGES **************/
	case WM_INPUT:
	{
		if (!m_Mouse.RawEnabled())
		{
			break;
		}
		UINT size;
		// first get the size of the input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1)
		{
			// bail msg processing if error
			break;
		}
		m_RawBuffer.resize(size);
		// read in the input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			m_RawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			// bail msg processing if error
			break;
		}
		// process the raw input data
		auto& ri = reinterpret_cast<const RAWINPUT&>(*m_RawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
		{
			m_Mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		break;
	}
	/************** END RAW MOUSE MESSAGES **************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}