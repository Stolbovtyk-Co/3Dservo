#include "ShaderManager.h"

void ShaderManager::Init(GraphicsDevice* gDevice, FileManager* flMgr)
{
	m_gDevice = gDevice;
	m_flMgr = flMgr;
}

void ShaderManager::CreateVertexShader(std::string PATH, std::string Name)
{
	if (!m_VS_DB.contains(Name)) {
		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> VS;

		auto dtoVertex = m_flMgr->ReadBytes(PATH);
		hr = m_gDevice->GetDevice()->CreateVertexShader(
			dtoVertex.Bytes,
			dtoVertex.BytesRead,
			nullptr,
			&VS
		);
		if (FAILED(hr)) {
			LOG_ERROR(m_logger, "Vertex shader creation failed : " + Name);
			m_logger.logHR(hr);
			return;
		}
		m_VS_DB.emplace(Name, VS);
		LOG_INFO(m_logger, "Vertex shader loaded : " + Name);
		CreateInputLayout(Name, dtoVertex);
	}
	else {
		LOG_WARN(m_logger, "Attempted to load already loaded vertex shader : " + Name);
	}
}

void ShaderManager::CreatePixelShader(std::string PATH, std::string Name)
{
	if (!m_PS_DB.contains(Name)) {
		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3D11PixelShader> PS;

		auto dtoVertex = m_flMgr->ReadBytes(PATH);
		hr = m_gDevice->GetDevice()->CreatePixelShader(
			dtoVertex.Bytes,
			dtoVertex.BytesRead,
			nullptr,
			&PS
		);
		if (FAILED(hr)) {
			LOG_ERROR(m_logger, "Pixel shader creation failed : " + Name);
			m_logger.logHR(hr);
			return;
		}
		m_PS_DB.emplace(Name, PS);
		LOG_INFO(m_logger, "Pixel shader loaded : " + Name);
	}
	else {
		LOG_WARN(m_logger, "Attempted to load already loaded pixel shader : " + Name);
	}
}

void ShaderManager::CreateInputLayout(std::string Name, FileManager::ReadedBytesDTO vShader)
{
	if (!m_IL_DB.contains(Name)) {
		HRESULT hr = S_OK;

		D3D11_INPUT_ELEMENT_DESC iaDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
				  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,
					0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		Microsoft::WRL::ComPtr<ID3D11InputLayout> IL;

		hr = m_gDevice->GetDevice()->CreateInputLayout(
			iaDesc,
			ARRAYSIZE(iaDesc),
			vShader.Bytes,
			vShader.BytesRead,
			&IL
		);
		if (FAILED(hr)) {
			LOG_ERROR(m_logger, "Input layout creation failed : " + Name);
			m_logger.logHR(hr);
			return;
		}

		m_IL_DB.emplace(Name, IL);
		LOG_INFO(m_logger, "Input layout loaded : " + Name);
	}
	else {
		LOG_WARN(m_logger, "Attempted to load already loaded input layout : " + Name);
	}
}

void ShaderManager::CreateSampler(std::string Name)
{
	if (!m_Sampler_DB.contains(Name)) {
		HRESULT hr = S_OK;

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; 
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;   
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;   
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;   
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

		hr = m_gDevice->GetDevice()->CreateSamplerState(&sampDesc, &sampler);
		if (FAILED(hr)) {
			LOG_ERROR(m_logger, "Sampler creation failed : " + Name);
			m_logger.logHR(hr);
			return;
		}

		m_Sampler_DB.emplace(Name, sampler);
		LOG_INFO(m_logger, "Sampler loaded : " + Name);
	}
	else {
		LOG_WARN(m_logger, "Attempted to load already loaded sampler : " + Name);
	}
}

void ShaderManager::SetVS(std::string name, bool fs)
{
	auto mat = m_VS_DB.find(name);
	if (mat != m_VS_DB.end()) {
		m_gDevice->GetContext()->VSSetShader(
			mat->second.Get(),
			nullptr,
			0
		);
	}
	else {
		LOG_ERROR(m_logger, "Attempted to set non existing VS : " + name);
		if (fs)
			SetVS("SV_DEFAULT", false);
	}
}

void ShaderManager::SetPS(std::string name, bool fs)
{
	auto mat = m_PS_DB.find(name);
	if (mat != m_PS_DB.end()) {
		m_gDevice->GetContext()->PSSetShader(
			mat->second.Get(),
			nullptr,
			0
		);
	}
	else {
		LOG_ERROR(m_logger, "Attempted to set non existing PS : " + name);
		if (fs)
			SetPS("SV_DEFAULT", false);
	}
}

void ShaderManager::SetIL(std::string name, bool fs)
{
	auto mat = m_IL_DB.find(name);
	if (mat != m_IL_DB.end()) {
		m_gDevice->GetContext()->IASetInputLayout(
			mat->second.Get()
		);
	}
	else {
		LOG_ERROR(m_logger, "Attempted to set non existing IL : " + name);
		if(fs)
			SetIL("SV_DEFAULT", false);
	}
}

void ShaderManager::SetSampler(std::string name, bool fs)
{
	auto mat = m_Sampler_DB.find(name);
	if (mat != m_Sampler_DB.end()) {
		ID3D11SamplerState* rawSampler = mat->second.Get();
		m_gDevice->GetContext()->PSSetSamplers(0, 1,
			&rawSampler
		);
	}
	else {
		LOG_ERROR(m_logger, "Attempted to set non existing sampler : " + name);
		if (fs)
			SetSampler("SV_DEFAULT", false);
	}
}
