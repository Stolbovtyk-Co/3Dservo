#pragma once
#include <string>
#include <vector>
#include "EngineConstants.h"

class ObjParser
{
public:
	struct ObjParserDTO {
		std::string name;
		Microsoft::WRL::ComPtr < ID3D11Buffer> vBuffer;
		Microsoft::WRL::ComPtr < ID3D11Buffer> iBuffer;
		long iCount;
	};
	ObjParserDTO ParseLines(std::vector<std::string> lines, Microsoft::WRL::ComPtr<ID3D11Device> com_device);
	
	std::vector<std::string> Split(std::string& str, char delimiter);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	long m_indexCount;
};

