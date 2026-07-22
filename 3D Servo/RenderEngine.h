#pragma once
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

class RenderEngine {
public:
    RenderEngine() = default;

    void Initialize(HWND hWnd, int width, int height);

    void Clear(float r, float g, float b, float a);

    void Render();

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
};
