#include "ObjParser.h"
#include <string>
#include <vector>
#include <sstream>
#include "EngineConstants.h"
#include <unordered_map>
#include <DirectXMath.h>

std::vector<EConst::CpuMesh> ObjParser::TxtToObj(const std::vector<std::string>& lines)
{
    std::vector <EConst::CpuMesh> cpuMeshes;
    std::vector<ObjParser::ObjO> objects = ObjParser::SplitObjToObjects(lines);
    for (auto& object : objects) {
        ObjOData objData = LoadObjOData(object.lines);
        std::vector<ObjParser::ObjMaterialGroup> mGroups = SplitByMaterial(object.lines);
        EConst::CpuMesh m = ObjOToMesh(objData, mGroups);
        m.OName = object.O_Name;
        cpuMeshes.push_back(m);
    }
    return cpuMeshes;
}

std::vector<std::string> ObjParser::GetRequiredMtlFiles(const std::vector<std::string>& lines)
{
    std::vector<std::string> mtlFiles;

    for (const std::string& line : lines)
    {
        std::vector<std::string> tokens = Split(line);

        if (tokens[0] != "mtllib" || tokens.size() < 2)
            continue;

        mtlFiles.push_back(tokens[1]);
    }

    return mtlFiles;
}

std::vector<ObjParser::ObjO> ObjParser::SplitObjToObjects(const std::vector<std::string>& lines)
{
    std::vector<ObjO> objects;

    ObjO current;
    current.O_Name = "Default";

    for (const std::string& line : lines)
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = Split(line);

        if (tokens.empty())
            continue;

        if (tokens[0] == "o")
        {

            if (!current.lines.empty())
                objects.push_back(current);

            current = ObjO();

            if (tokens.size() > 1)
                current.O_Name = tokens[1];
            else
                current.O_Name = "Unnamed";

            continue;
        }

        current.lines.push_back(line);
    }

    if (!current.lines.empty())
        objects.push_back(current);

    return objects;
}

std::vector<ObjParser::ObjMaterialGroup>
ObjParser::SplitByMaterial(const std::vector<std::string>& lines)
{
    std::vector<ObjMaterialGroup> groups;

    ObjMaterialGroup current;
    current.MaterialName = "default";

    for (const std::string& line : lines)
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = Split(line);

        if (tokens.empty())
            continue;

        if (tokens[0] == "usemtl")
        {
            if (!current.lines.empty())
                groups.push_back(current);

            current = ObjMaterialGroup();

            if (tokens.size() > 1)
                current.MaterialName = tokens[1];
            else
                current.MaterialName = "default";

            continue;
        }
        if (tokens[0] == "f")
            current.lines.push_back(line);
    }

    if (!current.lines.empty())
        groups.push_back(current);

    return groups;
}

ObjParser::ObjOData ObjParser::LoadObjOData(const std::vector<std::string>& Olines)
{
    ObjOData data;

    for (const std::string& line : Olines)
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = Split(line);

        if (tokens.empty())
            continue;

        // Vertex
        if (tokens[0] == "v")
        {
            if (tokens.size() < 4)
                continue;

            float w = 1.0f;

            if (tokens.size() >= 5)
                w = std::stof(tokens[4]);

            DirectX::XMFLOAT3 pos;
            pos.x = std::stof(tokens[1]) / w;
            pos.y = std::stof(tokens[2]) / w;
            pos.z = std::stof(tokens[3]) / w;

            data.pos.push_back(pos);

            // W. D. Gaster's freedom default color
            DirectX::XMFLOAT4 color = { 0.f, 1.f, 0.f, 1.f };

            if (tokens.size() >= 8)
            {
                color.x = std::stof(tokens[5]);
                color.y = std::stof(tokens[6]);
                color.z = std::stof(tokens[7]);
            }

            if (tokens.size() >= 9)
            {
                color.w = std::stof(tokens[8]);
            }

            data.colors.push_back(color);
        }

        // UV
        else if (tokens[0] == "vt")
        {
            if (tokens.size() < 3)
                continue;

            DirectX::XMFLOAT2 uv;
            uv.x = std::stof(tokens[1]);
            uv.y = std::stof(tokens[2]);

            data.uv.push_back(uv);
        }

        // Normal
        else if (tokens[0] == "vn")
        {
            if (tokens.size() < 4)
                continue;

            DirectX::XMFLOAT3 normal;
            normal.x = std::stof(tokens[1]);
            normal.y = std::stof(tokens[2]);
            normal.z = std::stof(tokens[3]);

            data.normals.push_back(normal);
        }
    }

    return data;
}

EConst::CpuMesh ObjParser::ObjOToMesh(const ObjOData& data, const std::vector<ObjMaterialGroup>& materialGroups)
{
    EConst::CpuMesh mesh;

    std::unordered_map<std::string, uint32_t> vertexMap;

    for (const ObjMaterialGroup& group : materialGroups)
    {
        uint32_t startIndex = static_cast<uint32_t>(mesh.Indices.size());

        for (const std::string& line : group.lines)
        {
            if (line.empty() || line[0] == '#')
                continue;

            std::vector<std::string> tokens = Split(line);

            if (tokens.empty() || tokens[0] != "f")
                continue;

            std::vector<uint32_t> faceIndices;

            for (size_t i = 1; i < tokens.size(); ++i)
            {
                auto it = vertexMap.find(tokens[i]);

                if (it != vertexMap.end())
                {
                    faceIndices.push_back(it->second);
                    continue;
                }

                int posIndex = -1;
                int uvIndex = -1;
                int normalIndex = -1;

                std::vector<std::string> parts = Split(tokens[i], '/');

                if (!parts.empty() && !parts[0].empty())
                    posIndex = std::stoi(parts[0]) - 1;

                if (parts.size() >= 2 && !parts[1].empty())
                    uvIndex = std::stoi(parts[1]) - 1;
                if (parts.size() >= 3 && !parts[2].empty())
                    normalIndex = std::stoi(parts[2]) - 1;

                EConst::VertexInfo vertex{};

                if (posIndex >= 0 && posIndex < data.pos.size())
                {
                    vertex.pos = data.pos[posIndex];

                    if (posIndex < data.colors.size())
                        vertex.color = data.colors[posIndex];
                    else
                        vertex.color = DirectX::XMFLOAT4(1, 1, 1, 1);
                }
                else
                {
                    continue;
                }

                if (uvIndex >= 0 && uvIndex < data.uv.size())
                    vertex.uv = data.uv[uvIndex];
                else
                    vertex.uv = DirectX::XMFLOAT2(0, 0);

                if (normalIndex >= 0 && normalIndex < data.normals.size())
                    vertex.normal = data.normals[normalIndex];
                else
                    vertex.normal = DirectX::XMFLOAT3(0, 0, 0);

                uint32_t vertexIndex = static_cast<uint32_t>(mesh.Vertices.size());

                mesh.Vertices.push_back(vertex);

                vertexMap.emplace(tokens[i], vertexIndex);

                faceIndices.push_back(vertexIndex);
            }

            // Triangulate polygon
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
            {
                mesh.Indices.push_back(faceIndices[0]);
                mesh.Indices.push_back(faceIndices[i]);
                mesh.Indices.push_back(faceIndices[i + 1]);
            }
        }

        uint32_t indexCount =
            static_cast<uint32_t>(mesh.Indices.size()) - startIndex;

        if (indexCount > 0)
        {
            mesh.SubMeshes.push_back({
                group.MaterialName,
                startIndex,
                indexCount
                });
        }
    }

    return mesh;
}

std::vector<std::string> ObjParser::Split(const std::string& str)
{
    std::stringstream ss(str);
    std::vector<std::string> tokens;
    std::string token;

    while (ss >> token)
        tokens.push_back(token);

    return tokens;
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
