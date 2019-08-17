#pragma once

#include <queue>
#include <string>
#include <windows.h>
#include <d3d11.h>

#define GRAPHICSDEVICE ID3D11Device*
#define GRAPHICSCONTEXT ID3D11DeviceContext*

class Direct3DEnvironment
{
public:
	Direct3DEnvironment(HWND windowHandle,
		unsigned mainBufferWidth, unsigned mainBufferHeight, bool vsync = false,
		bool enableMultisample = false);

	~Direct3DEnvironment();
	
	GRAPHICSDEVICE  GetDevice() { return m_device; }
	GRAPHICSCONTEXT GetContext() { return m_context; }

	void SetClearColor(float color[4]);

	void Clear();
	void Present();

	bool Init();

private:
	bool CreateDeviceAndContext();
	bool CreateSwapChain();
	bool CreateDepthAndStencilBuffers();
	bool SetViewport();
	bool SetRasterizerContext();

	unsigned m_mainBufferWidth, m_mainBufferHeight;

	HWND m_windowHandle;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	IDXGISwapChain* m_swapChain;
	
	ID3D11RenderTargetView* m_backBufferRenderTargetView;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterizerState;

	float m_clearColor[4];
	UINT m_ms4aQualityLevel;
	bool m_vsync, m_multisample;

	std::queue<std::string> m_errorQueue;
};