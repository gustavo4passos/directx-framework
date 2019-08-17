#pragma once

#include <string>
#include <d3d11.h>
#include <windows.h>

#define WINDOWHANDLE HWND

class Window
{
public:
	static Window* Get()
	{
		static Window* instance = new Window();
		return instance;
	}

	bool Init(unsigned width, unsigned height);
	bool Update();

	unsigned Width()  const { return m_width;  }
	unsigned Height() const { return m_height; }
	WINDOWHANDLE GetHandle() const { return m_hndWnd; }

	const std::string& Windowtitle() const { return m_windowTitle; }
	const bool DestructionRequested() const { return m_destructionRequested; }


private:
	Window();

	unsigned m_width;
	unsigned m_height;
	static bool m_initialized;

	bool m_destructionRequested;

	WINDOWHANDLE m_hndWnd;

	std::string m_windowTitle;

	static LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lparam);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool ShowQuitDialog();
};