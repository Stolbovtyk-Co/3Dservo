#pragma once
#include "EngineConstants.h"
#include <vector>

class MtlParser
{
public:
	std::vector <EConst::MaterialData> TxtToMtl(const std::vector<std::string>& lines);
private:
	struct MtlLinesSplited {
		std::string Name;
		std::vector<std::string> Lines;
	};
	std::vector<MtlLinesSplited> SplitToMaterials(const std::vector<std::string>& lines);

	static std::vector<std::string> Split(const std::string& str);
	EConst::MaterialData ParseMaterialSettings(const std::vector<std::string>& lines);
};

