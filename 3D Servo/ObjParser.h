#pragma once
#include <string>
#include <vector>
#include "EngineConstants.h"

class ObjParser
{
public:

	static EConst::CpuMeshDTO ParseLines(std::vector<std::string> lines);
	
	static std::vector<std::string> Split(std::string& str, char delimiter);
};

