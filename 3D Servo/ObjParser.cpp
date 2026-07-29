#include "ObjParser.h"
#include <string>
#include <vector>
#include <map>
#include "EngineConstants.h"
#include <DirectXMath.h>

EConst::CpuMeshDTO ObjParser::ParseLines(std::vector<std::string> lines)
{
    std::map<std::string, int> objMapKeyword = {
        { "s", -1},
        { "v", 0 },
        { "o", 1 },

        { "f", 2 }
    };

    EConst::CpuMeshDTO outputDTO;
    
    for (auto &text : lines) {
        std::vector<std::string> sp_string = Split(text, ' ');
        std::string kw = sp_string[0];
        auto it = objMapKeyword.find(kw);
        if (it != objMapKeyword.end()) {
            switch (it->second) {
            case 0: {
                DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
                if (sp_string.size() == 9) {
                    color.w = std::stof(sp_string[8]);
                }
                if (sp_string.size() >= 8) {
                    color.x = std::stof(sp_string[5]);
                    color.y = std::stof(sp_string[6]);
                    color.z = std::stof(sp_string[7]);
                }
                float w = 1.0f;
                DirectX::XMFLOAT3 cord = {};
                if (sp_string.size() >= 5) {
                    w = std::stof(sp_string[4]);
                }
                cord.x = std::stof(sp_string[1]) / w;
                cord.y = std::stof(sp_string[2]) / w;
                cord.z = std::stof(sp_string[3]) / w;

                EConst::VertexPositionColor cV = {};
                cV.color = color;
                cV.pos = cord;
                outputDTO.Vertices.push_back(cV);
                break;
            }

            case 1: {
                outputDTO.Name = sp_string[1];
                break;
            }

            case 2: {
                if (sp_string.size() == 4) {
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[1]) - 1));
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[2]) - 1));
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[3]) - 1));
                }
                else if (sp_string.size() == 5) {
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[1]) - 1));
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[2]) - 1));
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[3]) - 1));
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[1]) - 1));
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[3]) - 1));
                    outputDTO.Indices.push_back(static_cast<short>(std::stoi(sp_string[4]) - 1));
                }
                break;
            }
            };
        }
    }

    return outputDTO;
}

std::vector<std::string> ObjParser::Split(std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::size_t start = 0;
    std::size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    tokens.push_back(str.substr(start));
    return tokens;
}
