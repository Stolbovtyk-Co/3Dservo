#include <dwmapi.h>
#include "Application.h"

#pragma comment(lib, "dwmapi.lib")

//Servo3D might not be the best name for a 3D engine. 
//I would have thought that this stuff is for Arduino users.
std::wstring Application::m_windowClassName = L"ServoOverlayClass"; 

Application::Application() {
    Initialize();
    
    m_isRunning = true;
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
    wc.lpszClassName = m_windowClassName.c_str();
    wc.hbrBackground = (HBRUSH)GetStockObject(TRANSPARENT); 

    RegisterClassExW(&wc); 

    m_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,  // Overlay Flags
        m_windowClassName.c_str(), L"Overlay",
        WS_POPUP, // No frame
        0, 0, WIN_WIDTH, WIN_HEIGHT, // Size
        nullptr, nullptr, hInstance, nullptr
    );
    #pragma endregion

    ShowWindow(m_hWnd, SW_SHOW);

    m_renderEngine = std::make_unique<RenderEngine>(m_logger, m_hWnd, WIN_WIDTH, WIN_HEIGHT);
    m_renderEngine->PreloadAssetsAsync();
}

void Application::Run() {
	while (m_isRunning) {
		Process();
		Risovat();
	}
}

/// <summary>
/// Scary Method For Scary System, Don't open!
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
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT) {
            m_isRunning = false;
        }
    }
    m_renderEngine->Update();
}

void Application::Risovat() {
    
    //m_renderEngine->Clear(0.0f, 1.0f, 0.0f, 0.5f);
    m_renderEngine->Render();
}