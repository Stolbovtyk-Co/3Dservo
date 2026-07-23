#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <dxgi1_3.h>

#pragma comment(lib, "d3d11.lib")

class RenderEngine {
public:
    RenderEngine() = default;

    void Initialize(HWND hWnd, int width, int height);

    void Clear(float r, float g, float b, float a);
    
    void Render();

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context = nullptr;
    Microsoft::WRL::ComPtr <IDXGISwapChain> m_swapChain = nullptr; //IDXGISwapChain Probably should be IDXGISwapChain1 to support alpha IDK
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_pBackBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget = nullptr;
    D3D11_TEXTURE2D_DESC m_bbDesc;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_pDepthStencil = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;
    D3D11_VIEWPORT m_viewport;

};