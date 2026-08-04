#pragma once
#include "EngineConstants.h"
#include "FileManager.h"
#include "MtlParser.h"
#include <unordered_map>
#include "Logger.h"
#include <vector>
#include <wrl/client.h>

class mtlManager
{
public:
	mtlManager(FileManager* flMgr, Microsoft::WRL::ComPtr<ID3D11Device> comDevice) {
		m_flMgr = flMgr;
		m_comDevice = comDevice;
		CreateMaterialConstantBuffer(m_comDevice.Get());
		AddNewMtlFile("engineMaterials.mtl");
	}
	void AddNewMtlFiles(std::vector<std::string> names);
	void AddNewMtlFile(std::string name);

	const EConst::DX11Material& GetMaterial(std::string name);

	inline ID3D11Buffer* GetTBuffer() {
		return m_Buffer.Get();
	}
private:
	Microsoft::WRL::ComPtr <ID3D11Buffer> m_Buffer;

	void CreateMaterialConstantBuffer(ID3D11Device* pDevice);

	EConst::DX11Material LoadMaterialToGPU(EConst::MaterialData& matData);
	std::vector<std::string> m_knownMtlFiles;
	FileManager* m_flMgr;
	MtlParser m_mtlParser;
	std::unordered_map<std::string, EConst::DX11Material> m_materialDB;
	Logger m_logger;
	//---------------------------------------
	//	IMPORTED FROM RENDER ENGINE CLASSES:
	Microsoft::WRL::ComPtr<ID3D11Device> m_comDevice;
};

