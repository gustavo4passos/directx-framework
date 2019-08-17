#include "Direct3DEnvironment.h"

#include <assert.h>

Direct3DEnvironment::Direct3DEnvironment(HWND windowHandle,
	unsigned mainBufferWidth,
	unsigned mainBufferHeight,
	bool vsync,
	bool enableMutisample)
	: m_windowHandle(windowHandle),
	m_mainBufferWidth(mainBufferWidth),
	m_mainBufferHeight(mainBufferHeight),
	m_device(nullptr),
	m_context(nullptr),
	m_swapChain(nullptr),
	m_backBufferRenderTargetView(nullptr),
	m_depthStencilView(nullptr),
	m_rasterizerState(nullptr),
	m_ms4aQualityLevel(0),
	m_vsync(vsync),
	m_multisample(enableMutisample)
{
	for (int i = 0; i < 4; i++) m_clearColor[i] = 0.25;
}

Direct3DEnvironment::~Direct3DEnvironment()
{
	m_depthStencilView->Release();
	m_backBufferRenderTargetView->Release();
	m_swapChain->Release();
	m_device->Release();
	m_context->Release();
}

void Direct3DEnvironment::SetClearColor(float color[4])
{
	for (int i = 0; i < 4; i++) m_clearColor[i] = color[i];
}

void Direct3DEnvironment::Clear()
{
	m_context->ClearRenderTargetView(m_backBufferRenderTargetView, m_clearColor);
	//m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.F, 0.F);
}

void Direct3DEnvironment::Present()
{
	m_swapChain->Present(m_vsync ? 1 : 0, 0);
}

bool Direct3DEnvironment::Init()
{
	if (!CreateDeviceAndContext()) return false;
	if (!CreateSwapChain()) return false;
	if (!CreateDepthAndStencilBuffers()) return false;
	if (!SetViewport()) return false;
	if (!SetRasterizerContext()) return false;

	return true;
}

bool Direct3DEnvironment::CreateDeviceAndContext()
{
	UINT flags = 0;
#if defined(DEBUG) || defined(M_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&m_device,
		&featureLevel,
		&m_context
	);

	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to create context and device.");
		return false;
	}

	if(featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		m_errorQueue.push("Unable to create Direct3D 11 device.");
		return false;
	}
	return true;
}

bool Direct3DEnvironment::CreateSwapChain()
{
	assert(m_device != nullptr);
	assert(m_windowHandle != nullptr);

	if(FAILED(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 
		m_multisample ? 4 : 1, &m_ms4aQualityLevel) == 0)) 
	{
		MessageBoxW(nullptr, L"Multisample 4x not supported", L"Error", 0);
	}

	if (m_ms4aQualityLevel == 0)
	{
		m_errorQueue.push("Adapter does not support MS4AA.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

	sd.BufferDesc.Width = m_mainBufferWidth;
	sd.BufferDesc.Height = m_mainBufferHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	sd.SampleDesc.Count = m_multisample ? 4 : 1;
	sd.SampleDesc.Quality =  m_ms4aQualityLevel - 1;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = m_windowHandle;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	IDXGIDevice* dxgiDevice = nullptr;
	HRESULT hr = m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to query DXGIDevice interface com D3D11Device.");
		return false;
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)& dxgiAdapter);

	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to query DXDIAdapter interface com IDXGIDevice.");
		return false;
	}

	IDXGIFactory* dxgiFactory = nullptr;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to query DXDIFactory interface com DXDIAdapter.");
		return false;
	}

	hr = dxgiFactory->CreateSwapChain(
		m_device,
		&sd,
		&m_swapChain);

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to query DXDIFactory interface com DXDIAdapter.");
		return false;
	}

	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& backBuffer);
	
	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to get back buffer from swap chain.");
		return false;
	}

	hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_backBufferRenderTargetView);
	
	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to create render target view from back buffer.");
		return false;
	}

	m_context->OMSetRenderTargets(1, &m_backBufferRenderTargetView, m_depthStencilView);

	backBuffer->Release();

	return true;
}

bool Direct3DEnvironment::CreateDepthAndStencilBuffers()
{
	D3D11_TEXTURE2D_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_TEXTURE2D_DESC));

	dsDesc.Width = m_mainBufferWidth;
	dsDesc.Height = m_mainBufferHeight;
	dsDesc.MipLevels = 1; // Multi-sampled texture
	dsDesc.ArraySize = 1;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	dsDesc.SampleDesc.Count = m_multisample ? 4 : 1;
	dsDesc.SampleDesc.Quality = m_ms4aQualityLevel - 1;
	
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.MiscFlags = 0;

	ID3D11Texture2D* depthStencilBuffer;
	
	HRESULT hr = m_device->CreateTexture2D(&dsDesc, nullptr, &depthStencilBuffer);
	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to create depth-stencil texture buffer.");
		return false;
	}

	hr = m_device->CreateDepthStencilView(depthStencilBuffer, nullptr, &m_depthStencilView);
	if (FAILED(hr))
	{
		m_errorQueue.push("Unable to create depth-stencil view from buffer.");
		return false;
	}

	m_context->OMSetRenderTargets(1, &m_backBufferRenderTargetView, nullptr);

	return true;
}

bool Direct3DEnvironment::SetViewport()
{
	D3D11_VIEWPORT vp = { 0 };
	vp.Width = (FLOAT)m_mainBufferWidth;
	vp.Height = (FLOAT)m_mainBufferHeight;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MaxDepth = 1.f;
	vp.MinDepth = 0.f;

	m_context->RSSetViewports(1, &vp);

	return true;
}

bool Direct3DEnvironment::SetRasterizerContext()
{
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));

	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = TRUE; 

	HRESULT hr = m_device->CreateRasterizerState(&rd, &m_rasterizerState);

	if(FAILED(hr))
	{
		m_errorQueue.push("Unable to create default rasterizer state.");
		return false;
	}

	m_context->RSSetState(m_rasterizerState); 

	return true;
}