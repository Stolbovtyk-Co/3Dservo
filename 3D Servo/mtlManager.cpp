#include "mtlManager.h"
#include <algorithm>
#include <directxtk/WICTextureLoader.h>

void mtlManager::AddNewMtlFiles(std::vector<std::string> names)
{
	for (auto& name : names) {
		AddNewMtlFile(name);
	}
}

void mtlManager::AddNewMtlFile(std::string name)
{
	if (std::find(m_knownMtlFiles.begin(), m_knownMtlFiles.end(), name) == m_knownMtlFiles.end()) {
		//Load Mtl
		std::vector<std::string> mtlFileLines = m_flMgr->ReadText(name);
		std::vector<EConst::MaterialData> materials = m_mtlParser.TxtToMtl(mtlFileLines);
		for (auto& matData : materials) {
			m_materialDB.emplace(matData.name, LoadMaterialToGPU(matData));
		}
		m_knownMtlFiles.push_back(name);
	}
}

const EConst::DX11Material& mtlManager::GetMaterial(std::string name)
{
	auto mat = m_materialDB.find(name);
	if (mat != m_materialDB.end()) {
		return mat->second;
	}
	else {
		throw std::out_of_range("Material not found");
	}
}

EConst::DX11Material mtlManager::LoadMaterialToGPU(EConst::MaterialData& matData)
{
    EConst::DX11Material material{};

    // Constant buffer
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(EConst::MaterialBuffer);
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = &(matData.Settings);

    HRESULT hr = m_comDevice->CreateBuffer(
        &bd,
        &initData,
        material.vBuffer.GetAddressOf());

    if (FAILED(hr))
        throw std::runtime_error("Failed to create material constant buffer.");
    // Diffuse texture
    if (!matData.DiffuseTextureFile.empty())
    {
        DirectX::CreateWICTextureFromFile(
            m_comDevice.Get(),
            std::wstring(
                matData.DiffuseTextureFile.begin(),
                matData.DiffuseTextureFile.end()).c_str(),
            nullptr,
            material.Texture.GetAddressOf());
    }
    // Specular texture
    if (!matData.SpecularTextureFile.empty())
    {
        DirectX::CreateWICTextureFromFile(
            m_comDevice.Get(),
            std::wstring(
                matData.SpecularTextureFile.begin(),
                matData.SpecularTextureFile.end()).c_str(),
            nullptr,
            material.SpecularMap.GetAddressOf());
    }
    // Normal map
    if (!matData.NormalTextureFile.empty())
    {
        DirectX::CreateWICTextureFromFile(
            m_comDevice.Get(),
            std::wstring(
                matData.NormalTextureFile.begin(),
                matData.NormalTextureFile.end()).c_str(),
            nullptr,
            material.NormalMap.GetAddressOf());
    }

    return material;
}
