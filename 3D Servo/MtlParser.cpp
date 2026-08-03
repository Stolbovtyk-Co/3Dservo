#include "MtlParser.h"
#include <sstream>

std::vector <EConst::MaterialData> MtlParser::TxtToMtl(const std::vector<std::string>& lines)
{
    std::vector <EConst::MaterialData> output;
    std::vector<MtlParser::MtlLinesSplited> materialLines = SplitToMaterials(lines);
    for (auto& mat : materialLines) {
        EConst::MaterialData mData = ParseMaterialSettings(mat.Lines);
        mData.name = mat.Name;
        output.push_back(mData);
    }
    return output;
}

std::vector<MtlParser::MtlLinesSplited> MtlParser::SplitToMaterials(const std::vector<std::string>& lines)
{
    std::vector<MtlLinesSplited> objects;

    MtlLinesSplited current;
    current.Name = "Default";

    for (const std::string& line : lines)
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = Split(line);

        if (tokens.empty())
            continue;

        if (tokens[0] == "newmtl")
        {

            if (!current.Lines.empty())
                objects.push_back(current);

            current = MtlLinesSplited();

            if (tokens.size() > 1)
                current.Name = tokens[1];
            else
                current.Name = "Unnamed";

            continue;
        }

        current.Lines.push_back(line);
    }

    if (!current.Lines.empty())
        objects.push_back(current);

    return objects;
}

std::vector<std::string> MtlParser::Split(const std::string& str)
{
    std::stringstream ss(str);
    std::vector<std::string> tokens;
    std::string token;

    while (ss >> token)
        tokens.push_back(token);

    return tokens;
}

EConst::MaterialData MtlParser::ParseMaterialSettings(const std::vector<std::string>& lines)
{
    EConst::MaterialData output{};
    EConst::MaterialBuffer material{};

    material.ambientColor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    material.diffuseColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    material.specularColor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    material.shininess = 0.0f;

    for (const std::string& line : lines)
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = Split(line);

        if (tokens.empty())
            continue;

        if (tokens[0] == "Ka")
        {
            if (tokens.size() >= 4)
            {
                material.ambientColor.x = std::stof(tokens[1]);
                material.ambientColor.y = std::stof(tokens[2]);
                material.ambientColor.z = std::stof(tokens[3]);
            }
        }
        else if (tokens[0] == "Kd")
        {
            if (tokens.size() >= 4)
            {
                material.diffuseColor.x = std::stof(tokens[1]);
                material.diffuseColor.y = std::stof(tokens[2]);
                material.diffuseColor.z = std::stof(tokens[3]);
            }
        }
        else if (tokens[0] == "Ks")
        {
            if (tokens.size() >= 4)
            {
                material.specularColor.x = std::stof(tokens[1]);
                material.specularColor.y = std::stof(tokens[2]);
                material.specularColor.z = std::stof(tokens[3]);
            }
        }
        else if (tokens[0] == "Ns")
        {
            if (tokens.size() >= 2)
                material.shininess = std::stof(tokens[1]);
        }
        else {
            if (tokens[0] == "map_Kd")
                if (tokens.size() >= 2)
                    output.DiffuseTextureFile = tokens[1];

            if (tokens[0] == "map_Ks")
                if (tokens.size() >= 2)
                    output.SpecularTextureFile = tokens[1];

            if (tokens[0] == "map_Bump" || tokens[0] == "bump")
                if (tokens.size() >= 2)
                    output.NormalTextureFile = tokens[1];
        }
    }
    output.Settings = material;
    return output;
}
