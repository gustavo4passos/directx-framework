#define UNICODE
#include "Application.h"
#include "Window.h"
#include "Shader.h"


Application::Application()
	: m_d3de(nullptr)
{
}

Application::~Application()
{
	delete m_d3de;
	m_d3de = nullptr;
}

bool Application::Init()
{
	BOOL successfullyChangedDirectory = SetCurrentDirectory(L"../");
	if(successfullyChangedDirectory == 0)
	{
		MessageBox(Window::Get()->GetHandle(), L"Unable to change current directory", L"Error", 0);
	}
	
	unsigned windowWidth = 1366;
	unsigned windowHeight = 768;

	if (!Window::Get()->Init(windowWidth, windowHeight)) return false;
	
	m_d3de = new Direct3DEnvironment(Window::Get()->GetHandle(), 
		windowWidth, windowHeight, true, true);

	if (!m_d3de->Init()) return false;

	return true;
}

int Application::Run()
{
	while (!Window::Get()->DestructionRequested())
	{
		Window::Get()->Update();
		m_d3de->Clear();

		Draw();

		m_d3de->Present();
	}

	return 0;
}
