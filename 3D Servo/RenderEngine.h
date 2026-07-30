#pragma once
#include <d3d11.h>
#include<dcomp.h>
#include <wrl/client.h>
#include "Logger.h"
#include "Scene.h"
#include "EngineConstants.h"
#include <concepts>
#include "RenderQueueManager.h"

#pragma comment(lib, "d3d11.lib")

class RenderEngine {
public:
    RenderEngine(std::shared_ptr<Logger> logger, HWND hWnd, int Width, int Height);

    void Initialize(HWND hWnd, int m_width, int m_height);
    
    void Render();

    void Update();

    void CreateViewAndPerspective();

    void PreloadAssetsAsync();

    template <typename T>
    void LoadScene() requires std::is_base_of_v<Scene, T> {
        m_currentScene = std::make_shared<T>(m_device);
    }

private:

    void Clear(float r, float g, float b, float a);
    void LoadSceneSettings();

    void RenderGPUBuffers(EConst::GPUBuffers g);

    std::shared_ptr <Scene> m_currentScene = nullptr;

    EConst::ConstantBufferStruct m_constantBufferData;

    float m_width;
    float m_height;
    DirectX::XMFLOAT4 m_clearColor;

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

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

    std::shared_ptr<Logger> m_logger;
    std::unique_ptr<RenderQueueManager> m_renderQueue;

    //Loaders
    //BufferLoader m_bufferLoader;

    //=========================\\
    //Direct3D device resources\\
    //=========================\\

    //Stencils
    ID3D11DepthStencilState* pDepthStencilStateRegular = nullptr;
    ID3D11DepthStencilState* pDepthStencilStateTransparent = nullptr;
    ID3D11BlendState* m_pBlendStateTransparent = nullptr;
    
    //Shaders
    Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            m_constantBuffer;
};