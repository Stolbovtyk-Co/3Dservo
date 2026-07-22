#include <dwmapi.h>
#include "Application.h"

#pragma comment(lib, "dwmapi.lib")

Application::Application() {
    Initialize();
}

void Application::Initialize() {
#pragma region Window Setting 
    // Open the window, it's stuffy here.

    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc; 
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ServoOverlayClass";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 

    RegisterClassExW(&wc); 

    hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, // Overlay Flags
        L"ServoOverlayClass", L"Overlay",
        WS_POPUP, // No frame
        0, 0, 800, 600, // Size
        nullptr, nullptr, hInstance, nullptr
    );

    // Invisibility
    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hWnd, &margins);

    // Show Window
    ShowWindow(hWnd, SW_SHOW);
#pragma endregion

    m_isRunning = true; 
}

void Application::Run() {
	while (m_isRunning) {
		Process();
		Risovat();
	}
}

/// <summary>
/// Scary Function For Scary System, Don't open!
/// </summary>
LRESULT CALLBACK Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0); 
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void Application::Process() {
    MSG msg = {};

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg); 

        if (msg.message == WM_QUIT) {

        }
    }
}

void Application::Risovat() {

}