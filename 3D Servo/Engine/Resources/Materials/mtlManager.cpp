#include "mtlManager.h"
#include <algorithm>
#include <directxtk/WICTextureLoader.h>
#include <wrl/client.h>

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
            auto dx11m = LoadMaterialToGPU(matData);
            m_materialDB.emplace(matData.name, dx11m);
            LOG_INFO(m_logger,"New material loaded : " + matData.name);
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
		throw std::out_of_range("Material not found(" + name + ")");
	}
}

void mtlManager::CreateMaterialConstantBuffer(ID3D11Device* pDevice)
{
    static_assert(sizeof(EConst::MaterialBuffer) % 16 == 0, "MaterialBuffer size must be a multiple of 16 bytes!");

    ID3D11Buffer* pBuffer = nullptr;

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(EConst::MaterialBuffer);       // Will be exactly 64 bytes
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;             // Optimized for CPU writes / GPU reads
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;   // Tells DX11 this is a constant buffer
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Allows the CPU to use Map() with WRITE_DISCARD
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    HRESULT hr = pDevice->CreateBuffer(&cbDesc, nullptr, &m_Buffer);

}

EConst::DX11Material mtlManager::LoadMaterialToGPU(EConst::MaterialData& matData)
{
    EConst::DX11Material material{};

    material.mBuffer = matData.Settings;

    

    // Diffuse texture
    if (!matData.DiffuseTextureFile.empty())
    {
        material.mBuffer.hasDiffuseMap = 1;
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
        material.mBuffer.hasSpecularMap = 1;
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
        material.mBuffer.hasNormalMap = 1;
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
