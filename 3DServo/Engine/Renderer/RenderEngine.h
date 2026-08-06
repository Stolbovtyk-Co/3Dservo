#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Core/Logger.h"
#include "Scene/Scene.h"
#include "EngineConstants.h"
#include "RenderQueueManager.h"
#include "Device/GraphicsDevice.h"
#include "Shaders/ShaderManager.h"

#pragma comment(lib, "d3d11.lib")

class RenderEngine {
public:
    RenderEngine(HWND hWnd, int Width, int Height);

    void Initialize(HWND hWnd, int m_width, int m_height);
    
    void Render();

    void Update();

    void CreateViewAndPerspective();

    void PreloadAssetsAsync();

    template <typename T>
    void LoadScene() requires std::is_base_of_v<Scene, T> {
        m_currentScene = std::make_shared<T>(m_gDevice.GetDevice());
        LoadSceneSettings();
    }

private:
    void LoadSceneSettings();

    void RenderGPUBuffers(EConst::GPUBuffers g);

    std::shared_ptr <Scene> m_currentScene = nullptr;

    EConst::ConstantBufferStruct m_constantBufferData;

    DirectX::XMFLOAT4 m_clearColor;

    Logger m_logger;
    std::unique_ptr<RenderQueueManager> m_renderQueue;
    GraphicsDevice m_gDevice;
    ShaderManager m_sMgr;
    FileManager m_flMgr;

    float m_width;
    float m_height;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    
};