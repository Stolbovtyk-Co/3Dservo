#define NOMINMAX 1 
#include "ConvexDecomposer.h"

#define ENABLE_VHACD_IMPLEMENTATION 1
#include <VHACD.h> 
#include <random>

std::vector<EConst::CpuMeshDTO> ConvexDecomposer::runConvexDecomposition(EConst::CpuMeshDTO dto)
{
    std::vector<EConst::CpuMeshDTO> result;
    vhacdMeshDTO Mesh = ConvertToVHACD(dto);

    uint32_t countPoints = static_cast<uint32_t>(dto.Vertices.size());
    uint32_t countTriangles = static_cast<uint32_t>(dto.Indices.size() / 3);

    VHACD::IVHACD* interfaceVHACD = VHACD::CreateVHACD();
    VHACD::IVHACD::Parameters params;
    params.m_resolution = 50000;
    params.m_maxConvexHulls = 12;

    bool success = interfaceVHACD->Compute(
        Mesh.Points.data(), countPoints,
        Mesh.Triangles.data(), countTriangles,
        params
    );

    if (success) {
        uint32_t numHulls = interfaceVHACD->GetNConvexHulls();

        for (uint32_t i = 0; i < numHulls; ++i) {
            VHACD::IVHACD::ConvexHull hull;
            interfaceVHACD->GetConvexHull(i, hull);

            EConst::CpuMeshDTO partDto = ConvertToMesh(&hull);
            partDto.Name = dto.Name + "_hull_" + std::to_string(i);

            result.push_back(partDto);

            if (hull.m_points.empty() || hull.m_triangles.empty()) {
                std::cout << "[Warning] V-HACD generated an empty hull, skipping...\n";
                continue;
            }
        }
    }
    else {
        std::cout << "Пашёл нахуй козлина";
    }

    interfaceVHACD->Clean();
    interfaceVHACD->Release();

    return result;
}

ConvexDecomposer::vhacdMeshDTO ConvexDecomposer::ConvertToVHACD(EConst::CpuMeshDTO dto)
{
    vhacdMeshDTO result;
    result.Points.reserve(dto.Vertices.size() * 3);

    for (const auto& vertex : dto.Vertices) {
        result.Points.push_back(static_cast<double>(vertex.pos.x));
        result.Points.push_back(static_cast<double>(vertex.pos.y));
        result.Points.push_back(static_cast<double>(vertex.pos.z));
    }

    result.Triangles.reserve(dto.Indices.size());
    for (short index : dto.Indices) {
        result.Triangles.push_back(static_cast<uint32_t>(index));
    }

    return result;
}

EConst::CpuMeshDTO ConvexDecomposer::ConvertToMesh(void* hullPtr)
{
    EConst::CpuMeshDTO partDto;

    auto* hull = static_cast<VHACD::IVHACD::ConvexHull*>(hullPtr);

    partDto.Vertices.reserve(hull->m_points.size());
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);
    auto col = DirectX::XMFLOAT4(dis(gen), dis(gen), dis(gen), 0.5f);

    for (const auto& hPoint : hull->m_points) {
        EConst::VertexPositionColor vpc;

        vpc.pos.x = static_cast<float>(hPoint.mX);
        vpc.pos.y = static_cast<float>(hPoint.mY);
        vpc.pos.z = static_cast<float>(hPoint.mZ);
        vpc.color = col;

        partDto.Vertices.push_back(vpc);
    }

    partDto.Indices.reserve(hull->m_triangles.size() * 3);
    for (const auto& tri : hull->m_triangles) {
        partDto.Indices.push_back(static_cast<short>(tri.mI0));
        partDto.Indices.push_back(static_cast<short>(tri.mI1));
        partDto.Indices.push_back(static_cast<short>(tri.mI2));
    }

    return partDto;
}
