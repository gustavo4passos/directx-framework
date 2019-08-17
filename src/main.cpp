#include <windows.h>
#include <d3d11.h>

#include "TriangleApp.h"

int WINAPI
WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE prevInstance,
    _In_opt_ LPSTR strCmdLine,
    _In_ int show)
{/*
    TriangleApp app;
    if(!app.Init()) return 0;

    int result = app.Run();*/

    TriangleApp* app = new TriangleApp();
    if(!app->Init()) return 0;
    int result = app->Run();
    delete app;
    app = nullptr;

    return result;
}