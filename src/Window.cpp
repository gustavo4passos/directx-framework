#define UNICODE
#include "Window.h"

#include "Xinput.h"
#include <assert.h>

bool Window::m_initialized = false;

Window::Window()
	: m_width(0),
	m_height(0),
	m_destructionRequested(false),
	m_windowTitle(""),
	m_hndWnd(nullptr)
{
}

bool Window::Init(unsigned width, unsigned height)
{
	m_width = width;
	m_height = height;

	HINSTANCE currentInstance = (HINSTANCE)GetModuleHandle(nullptr);

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = this->DefaultWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = currentInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"BasicWndClass";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass failed", 0, 0);
		return false;
	}
	m_hndWnd = CreateWindow(
		L"BasicWndClass",
		L"Win32Basic",
		(WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_width,
		m_height,
		0,
		0,
		currentInstance,
		0);

	if (m_hndWnd == 0)
	{
		MessageBox(0, L"CreateWnd failed", 0, 0);
		return false;
	}

	/*
	if (SetWindowLongPtr(m_hndWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)) == 0)
	{
		MessageBox(0, L"SetWindowLongPtr failed!", L"Error", 0);
		return false;
	}
	*/

	ShowWindow(m_hndWnd, true);
	UpdateWindow(m_hndWnd);

	m_initialized = true;
	return true;
}

bool Window::Update()
{
	assert(m_initialized);

	static XINPUT_STATE gamepadState;
	DWORD controllerState = XInputGetState(0, &gamepadState);

	if(controllerState == ERROR_SUCCESS)
	{
		int a = gamepadState.Gamepad.bLeftTrigger;
		XINPUT_VIBRATION vib;
		ZeroMemory(&vib, sizeof(vib));
		if(gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
		{
			vib.wLeftMotorSpeed = 30000;
			vib.wRightMotorSpeed = 30000;
		}
		XInputSetState(0, &vib);
	}

	MSG msg = { 0 };

	while (!m_destructionRequested && PeekMessage(&msg, m_hndWnd, 0, 0, PM_REMOVE) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

LRESULT Window::DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*
	Window* wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (wnd) return wnd->WndProc(hWnd, msg, wParam, lParam);
	else return DefWindowProc(hWnd, msg, wParam, lParam);
	*/
	return Get()->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			if(ShowQuitDialog()) DestroyWindow(hWnd);
		}

		return 0;

	case WM_CLOSE:
	{
		if (ShowQuitDialog()) DestroyWindow(m_hndWnd);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		m_destructionRequested = true;
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Window::ShowQuitDialog()
{
	int quit = MessageBox(m_hndWnd, L"Do you really want to quit?", L"Quit?", MB_YESNO);
	if (quit == IDYES)
	{
		return true;
	}

	return false;
}
