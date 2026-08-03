#pragma once
#include "ObjParser.h"
#include "FileManager.h"
#include "mtlManager.h"
#include <unordered_map>
#include "Logger.h"
#include <vector>

class ObjectManager
{
public:
	ObjectManager(FileManager* flMgr, mtlManager* mtlMgr, Microsoft::WRL::ComPtr<ID3D11Device> comDevice) {
		m_flMgr = flMgr;
		m_mtlMgr = mtlMgr;
		m_comDevice = comDevice;
	}
	void LoadObjects(std::vector<std::string> objects);
	void LoadObject(std::string object);
	const std::vector<EConst::SubMesh>& GetObjectSubMeshes(std::string name, bool autoLoad = true);
private:
	EConst::SubMesh LoadObjectToGPU(const EConst::CpuMesh&);

	std::unordered_map<std::string,std::vector<EConst::SubMesh>> m_objectDB;
	std::vector<std::string> m_knownObjectFiles;

	Logger m_logger;

	FileManager* m_flMgr;
	mtlManager* m_mtlMgr;	
	ObjParser m_objParser;
	//---------------------------------------
	//	IMPORTED FROM RENDER ENGINE CLASSES:
	Microsoft::WRL::ComPtr<ID3D11Device> m_comDevice;
};

