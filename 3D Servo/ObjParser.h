#pragma once
#include <string>
#include <vector>
#include "EngineConstants.h"

class ObjParser
{
public:
	std::vector<EConst::CpuMesh> TxtToObj(const std::vector<std::string>& lines);
	std::vector<std::string> GetRequiredMtlFiles(const std::vector<std::string>& lines);
private:
	struct ObjO {
		std::string O_Name = "null";
		std::vector<std::string> lines;
	};
	std::vector<ObjParser::ObjO> SplitObjToObjects(const std::vector<std::string>& lines);

	struct ObjMaterialGroup
	{
		std::string MaterialName = "default";
		std::vector<std::string> lines;
	};
	std::vector<ObjParser::ObjMaterialGroup> SplitByMaterial(const std::vector<std::string>& lines);

	struct ObjOData {
		std::vector<DirectX::XMFLOAT3> pos;
		std::vector < DirectX::XMFLOAT4> colors;
		std::vector < DirectX::XMFLOAT2> uv;
		std::vector < DirectX::XMFLOAT3> normals;
	};
	ObjOData LoadObjOData(const std::vector<std::string>& Olines);

	EConst::CpuMesh ObjOToMesh(const ObjOData& data, const std::vector<ObjMaterialGroup>& materialGroups);

	static std::vector<std::string> Split(const std::string& str);
	static std::vector<std::string> Split(std::string& str, char delimiter);
};

