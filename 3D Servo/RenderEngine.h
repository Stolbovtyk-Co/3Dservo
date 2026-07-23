#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <dxgi1_3.h>
#include<dcomp.h>

#include "Logger.h"
#include "ShaderLoader.h"
#include "BufferLoader.h"

#pragma comment(lib, "d3d11.lib")

class ShaderLoader;
class BufferLoader;

class RenderEngine {
public:
    RenderEngine(std::shared_ptr<Logger> logger, HWND hWnd, int Width, int Height);

    void Initialize(HWND hWnd, int width, int height);

    void Clear(float r, float g, float b, float a);
    
    void Render();

    void Update();

    void CreateViewAndPerspective();

    void PreloadAssetsAsync();

private:
    ShaderLoader::ConstantBufferStruct m_constantBufferData;

    float width;
    float height;
    unsigned int  m_frameCount;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context = nullptr;
    Microsoft::WRL::ComPtr <IDXGISwapChain1> m_swapChain = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_pBackBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget = nullptr;
    D3D11_TEXTURE2D_DESC m_bbDesc;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_pDepthStencil = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;
    D3D11_VIEWPORT m_viewport;
    Microsoft::WRL::ComPtr<IDCompositionDevice> dcompDevice;
    Microsoft::WRL::ComPtr<IDCompositionTarget> dcompTarget;
    Microsoft::WRL::ComPtr<IDCompositionVisual> visual;

    std::shared_ptr<Logger> m_logger = nullptr;
    //Loaders
    ShaderLoader m_shaderLoader;
    BufferLoader m_bufferLoader;

    //=========================\\
    //Direct3D device resources\\
    //=========================\\
    
    //Shaders
    Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_constantBuffer;
    //Buffers
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
    //Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_pInputLayoutExtended;
};