#include "Scene.h"
#include "EngineConstants.h"

Scene::Scene(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	m_com_device = device;
	Setup();
}

void Scene::Setup()
{
	m_sh = std::make_unique<ShaderManager>(&m_fileMgr);
	m_st.clearColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_st.sceneName = "gm_Scene_Perfab";

	// Load Scene Tree
	m_bufferLoader.LoadBuffer(m_com_device);

	m_tree.SetName("tree");
	
	m_tree.SetObjFile("Scene.obj", &m_fileMgr, m_com_device);
	auto dto = m_bufferLoader.GetBuffersStaff();

	//m_tree.SetGPUBuffers(dto.VertexBuffer.Get(), dto.IndexBuffer.Get(), m_bufferLoader.GetIndexCount());
}

void Scene::Update(float delta)
{
}


Scene::ShaderManager::ShaderManager(FileManager* flMgr)
{
	m_flMgr = flMgr;
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

void Scene::ShaderManager::Setup(Microsoft::WRL::ComPtr<ID3D11Device> com_device)
{
	ID3D11Device* device = com_device.Get();
	HRESULT hr = S_OK;;

	FILE* vShader, * pShader;

	errno_t err;

	auto dtoVertex = m_flMgr->ReadBytes("VertexShader.cso");
	hr = device->CreateVertexShader(
		dtoVertex.Bytes,
		dtoVertex.BytesRead,
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
		dtoVertex.Bytes,
		dtoVertex.BytesRead,
		&m_inputLayout
	);

	auto dtoPixels = m_flMgr->ReadBytes("PixelShader.cso");
	hr = device->CreatePixelShader(
		dtoPixels.Bytes,
		dtoPixels.BytesRead,
		nullptr,
		m_pixelShader.GetAddressOf()
	);



	CD3D11_BUFFER_DESC cbDesc(
		sizeof(EConst::ConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	);

	hr = device->CreateBuffer(
		&cbDesc,
		nullptr,
		m_constantBuffer.GetAddressOf()
	);
}
