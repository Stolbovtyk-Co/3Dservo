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

    m_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,  // Overlay Flags
        L"ServoOverlayClass", L"Overlay",
        WS_POPUP, // No frame
        0, 0, WIN_WIDTH, WIN_HEIGHT, // Size
        nullptr, nullptr, hInstance, nullptr
    );

    // Show Window
    ShowWindow(m_hWnd, SW_SHOW);
    #pragma endregion

    m_renderEngine.Initialize(m_hWnd, WIN_WIDTH, WIN_HEIGHT);

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
LRESULT CALLBACK Application::WindowProc(HWND m_hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0); 
        return 0;
    }
    return DefWindowProc(m_hWnd, message, wParam, lParam);
}

void Application::Process() {
    bool bGotMsg;
    MSG msg = {};
    PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
    while (WM_QUIT != msg.message) {
        bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
        if (bGotMsg) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    }
}

void Application::Risovat() {

}