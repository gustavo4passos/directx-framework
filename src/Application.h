#pragma once

#include "Direct3DEnvironment.h"

class Application
{
public:
	Application();
	virtual ~Application();
	
	virtual bool Init();
	virtual void Draw() = 0;
	virtual int Run();

protected:
	Direct3DEnvironment* m_d3de;
};