#include "Scene.h"
#include "EngineConstants.h"

Scene::Scene(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	m_com_device = device;
	Setup();
}

void Scene::Setup()
{


	// Load Scene Tree

	// SETUP SHADER ON CHILD LEVEL
}

void Scene::Update(float delta)
{
}

Scene::ShaderManager::ShaderManager(FileManager* flMgr, Microsoft::WRL::ComPtr<ID3D11Device> com_device)
{
	Scene::ShaderManager::Setup(flMgr, com_device);
}

Scene::ShaderManager::ShaderDTO Scene::ShaderManager::GetShaderManagerDTO()
{
	Scene::ShaderManager::ShaderDTO dto;
	dto.constantBuffer = Scene::ShaderManager::m_constantBuffer;
	dto.inputLayout = Scene::ShaderManager::m_inputLayout;
	dto.pixelShader = Scene::ShaderManager::m_pixelShader;
	dto.vertexShader = Scene::ShaderManager::m_vertexShader;
	return dto;
}

void Scene::ShaderManager::Setup(FileManager* flMgr, Microsoft::WRL::ComPtr<ID3D11Device> com_device)
{
	ID3D11Device* device = com_device.Get();
	HRESULT hr = S_OK;;

	FILE* vShader, * pShader;

	errno_t err;

	auto dto = flMgr->ReadBytes("VertexShader.hlsl");
	hr = device->CreateVertexShader(
		dto.Bytes,
		dto.BytesRead,
		nullptr,
		&m_vertexShader
	);

	D3D11_INPUT_ELEMENT_DESC iaDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		iaDesc,
		ARRAYSIZE(iaDesc),
		dto.Bytes,
		dto.BytesRead,
		&m_inputLayout
	);

	dto = flMgr->ReadBytes("VertexShader.hlsl");
	hr = device->CreatePixelShader(
		dto.Bytes,
		dto.BytesRead,
		nullptr,
		m_pixelShader.GetAddressOf()
	);

	EngineConstants eng;

	CD3D11_BUFFER_DESC cbDesc(
		eng.GetSizeOfConstantBufferStruct(),
		D3D11_BIND_CONSTANT_BUFFER
	);

	hr = device->CreateBuffer(
		&cbDesc,
		nullptr,
		m_constantBuffer.GetAddressOf()
	);
}
