#include "ObjParser.h"
#include <string>
#include <vector>
#include <ranges>
#include <map>
#include "EngineConstants.h";

ObjParser::ObjParserDTO ObjParser::ParseLines(std::vector<std::string> lines, Microsoft::WRL::ComPtr<ID3D11Device> com_device)
{
    ID3D11Device* device = com_device.Get();

    std::map<std::string, int> objMapKeyword = {
        { "s", -1},
        { "v", 0 },
        { "o", 1 },

        { "f", 2 }
    };

    ObjParser::ObjParserDTO outputDTO;
    std::vector<EConst::VertexPositionColor> vVect;
    std::vector<short> iVect;
    std::string oName = "unset";
    
    for (auto text : lines) {
        std::vector<std::string> sp_string = Split(text, ' ');
        std::string kw = sp_string[0];
        auto it = objMapKeyword.find(kw);
        if (it != objMapKeyword.end()) {
            switch (it->second) {
            case 0: {
                DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
                if (sp_string.size() == 9) {
                    color.w = std::stof(sp_string[8]);
                }
                if (sp_string.size() >= 8) {
                    color.x = std::stof(sp_string[5]);
                    color.y = std::stof(sp_string[6]);
                    color.z = std::stof(sp_string[7]);
                }
                float w = 1.0f;
                DirectX::XMFLOAT3 cord;
                if (sp_string.size() >= 5) {
                    w = std::stof(sp_string[4]);
                }
                cord.x = std::stof(sp_string[1]) / w;
                cord.y = std::stof(sp_string[2]) / w;
                cord.z = std::stof(sp_string[3]) / w;

                EConst::VertexPositionColor cV;
                cV.color = color;
                cV.pos = cord;
                vVect.push_back(cV);
                break;
            }

            case 1: {
                oName = sp_string[1];
                break;
            }

            case 2: {
                iVect.push_back(static_cast<short>(std::stoi(sp_string[1])));
                iVect.push_back(static_cast<short>(std::stoi(sp_string[2])));
                iVect.push_back(static_cast<short>(std::stoi(sp_string[3])));
                break;
            }
            };
        }
    }

    EConst::VertexPositionColor* CubeVertices = vVect.data();

    CD3D11_BUFFER_DESC vDesc(
        sizeof(CubeVertices),
        D3D11_BIND_VERTEX_BUFFER
    );

    D3D11_SUBRESOURCE_DATA vData;
    ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
    vData.pSysMem = CubeVertices;
    vData.SysMemPitch = 0;
    vData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(
        &vDesc,
        &vData,
        &m_vertexBuffer
    );

    short* CubeIndices = iVect.data();

    m_indexCount = iVect.size() * sizeof(short);

    CD3D11_BUFFER_DESC iDesc(
        sizeof(CubeIndices),
        D3D11_BIND_INDEX_BUFFER
    );

    D3D11_SUBRESOURCE_DATA iData;
    ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
    iData.pSysMem = CubeIndices;
    iData.SysMemPitch = 0;
    iData.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(
        &iDesc,
        &iData,
        &m_indexBuffer
    );
    
    outputDTO.name = oName;
    outputDTO.vBuffer = m_vertexBuffer;
    outputDTO.iBuffer = m_indexBuffer;
    outputDTO.iCount = m_indexCount;

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
