#include "RenderEngine.h"
#include "Core/Logger.h"
#include "Scene/Scene.h"
#include "EngineConstants.h"
#include <wrl/client.h>
#include <memory>
#include <d3d11.h> 
#include <dxgiformat.h>

using namespace DirectX;

#pragma comment(lib, "dcomp.lib")

#pragma region Initialization

RenderEngine::RenderEngine(HWND hWnd, int Width, int Height)
{
	Initialize(hWnd, Width, Height);
	LoadScene<Scene>();
	m_renderQueue = std::make_unique<RenderQueueManager>(m_currentScene, &m_constantBufferData);
}

void RenderEngine::Initialize(HWND hWnd, int Width, int Height)
{
	m_width = Width;
	m_height = Height;
	m_gDevice.Init(hWnd, Width, Height);
	m_sMgr.Init(&m_gDevice,&m_flMgr);
}

void RenderEngine::CreateViewAndPerspective()
{
	DirectX::XMVECTOR eye = m_currentScene->GetSceneSettings().cameraPos; //TODO: rework
	DirectX::XMVECTOR at = m_currentScene->GetSceneSettings().cameraLookAt;
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

	DirectX::XMStoreFloat4x4(
		&m_constantBufferData.view,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixLookAtRH( 
				eye,
				at,
				up
			)
		)
	);

	float aspectRatioX = m_width / m_height;
	float aspectRatioY = aspectRatioX < (16.0f / 9.0f) ? aspectRatioX / (16.0f / 9.0f) : 1.0f;

	DirectX::XMStoreFloat4x4(
		&m_constantBufferData.projection,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixPerspectiveFovRH(
				DirectX::XMConvertToRadians(70), 
				aspectRatioX,
				0.01f,
				100.0f
			)
		)
	);
}

#pragma endregion

#pragma region Update

void RenderEngine::Update()
{
	m_currentScene->Update(1); //TODO:fix
}

#pragma endregion

#pragma region Render


void RenderEngine::Render() {

	auto renderQueue = m_renderQueue->MakeRenderQueue();

	m_gDevice.FrameRenderBegin(m_clearColor);

	m_gDevice.FrameRenderRegular();
	
	for (auto &i : renderQueue.regular) {
		RenderGPUBuffers(i);
	}

	m_gDevice.FrameRenderTransparent();

	for (auto& i : renderQueue.transparent) {
		RenderGPUBuffers(i);
	}

	m_gDevice.FrameRenderFinish();
}

void RenderEngine::RenderGPUBuffers(EConst::GPUBuffers g)
{
	DirectX::XMMATRIX globalMat = g.worldMatrix;
	DirectX::XMStoreFloat4x4(&m_constantBufferData.world, DirectX::XMMatrixTranspose(globalMat));

	m_gDevice.GetContext()->UpdateSubresource(
		m_constantBuffer.Get(),
		0,
		nullptr,
		&m_constantBufferData,
		0,
		0
	);

	UINT stride = sizeof(EConst::VertexInfo);
	UINT offset = 0;
	m_gDevice.GetContext()->IASetVertexBuffers(
		0,
		1,
		g.vBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	m_gDevice.GetContext()->IASetIndexBuffer(
		g.iBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);

	m_gDevice.GetContext()->VSSetConstantBuffers(
		0,
		1,
		m_constantBuffer.GetAddressOf()
	);

	
	m_sMgr.SetSampler("SV_DEFAULT");

	for (const auto& subMesh : g.MaterialRegions) {
		auto& cMat = m_currentScene->GetMtlManager()->GetMaterial(subMesh.MaterialName);
		auto tBuffer = m_currentScene->GetMtlManager()->GetTBuffer();

		//TODO: send camera pos

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hrT = m_gDevice.GetContext()->Map(
			tBuffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource
		);
		if(SUCCEEDED(hrT)) {
			memcpy(mappedResource.pData, &(cMat.mBuffer), sizeof(EConst::MaterialBuffer));
			m_gDevice.GetContext()->Unmap(tBuffer, 0);
		}

		ID3D11ShaderResourceView* textureSRV = cMat.Texture.Get();
		m_gDevice.GetContext()->PSSetShaderResources(
			0,           
			1,            
			&textureSRV   
		);

		ID3D11ShaderResourceView* textureN = cMat.NormalMap.Get();
		m_gDevice.GetContext()->PSSetShaderResources(
			2,
			1,
			&textureN
		);

		ID3D11ShaderResourceView* textureS = cMat.SpecularMap.Get();
		m_gDevice.GetContext()->PSSetShaderResources(
			1,
			1,
			&textureS
		);

		m_gDevice.GetContext()->PSSetConstantBuffers(1, 1, &tBuffer);

		m_gDevice.GetContext()->DrawIndexed(
			subMesh.IndexCount,  
			subMesh.StartIndex,   
			0                     
		);
	}
}

#pragma endregion

#pragma region Scene

void RenderEngine::LoadSceneSettings()
{
	Scene::SceneSettings settings = m_currentScene.get()->GetSceneSettings();
	LOG_INFO(m_logger, "Loaded scene: " + settings.sceneName);
	m_clearColor = settings.clearColor;
	CreateViewAndPerspective();
}

/// TODO: async
void RenderEngine::PreloadAssetsAsync()
{
#pragma region Shaders

	m_sMgr.CreateVertexShader("VertexShader.cso", "SV_DEFAULT");
	m_sMgr.SetVS("SV_DEFAULT", false);

	m_sMgr.CreatePixelShader("PixelShader.cso", "SV_DEFAULT");
	m_sMgr.SetPS("SV_DEFAULT", false);
	m_sMgr.SetIL("SV_DEFAULT", false);

	m_sMgr.CreateSampler("SV_DEFAULT");

	CD3D11_BUFFER_DESC cbDesc(
		sizeof(EConst::ConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	);

	m_gDevice.GetDevice()->CreateBuffer(
		&cbDesc,
		nullptr,
		m_constantBuffer.GetAddressOf()
	);

	m_gDevice.GetContext()->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
#pragma endregion
}
#pragma endregion
