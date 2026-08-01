#include "MeshFactory.h"
#include <DirectXCollision.h>
#include "ObjParser.h"
#include "FileManager.h"
#include "MeshInstance3D.h"
#include "EngineConstants.h"
#include <d3d11.h>
#include "Obstacle3D.h"

std::shared_ptr<Node3D> MeshFactory::CreateStaticInstance(std::string PATH, bool doCollisionMeshes)
{
    auto rawData = m_fileManager->ReadText(PATH);

    if (rawData.empty()) {
        throw std::runtime_error("[FATAL|MeshFactory] No Data!");
    }

    auto cpu = ObjParser::ParseLines(rawData);  

    std::vector<EConst::SubMesh> VisualHulls; 
    VisualHulls.push_back(CreateSubMesh(cpu, CreateGPUBuffers(cpu), false));

    std::shared_ptr<MeshInstance3D> newInstance;

    if (doCollisionMeshes) {
        //TODO: ASYNCH HERE!!!! 
        auto obstacle = std::make_shared<Obstacle3D>();
        std::vector<EConst::SubMesh> CollisionHulls = GetStaticCollisionSubMeshes(PATH, cpu);
        obstacle->SetCollisionSubMeshes(CollisionHulls);
        newInstance = obstacle;
    }
    else {
        newInstance = std::make_shared<MeshInstance3D>();
    }
    newInstance->SetSubMeshes(VisualHulls);

    return newInstance;
}

std::vector<EConst::SubMesh> MeshFactory::GetStaticCollisionSubMeshes(std::string PATH, EConst::CpuMeshDTO cpu)
{
    if (m_gpuCache.count(PATH) > 0) {
        std::vector<EConst::SubMesh> result;
        result.reserve(m_gpuCache[PATH].size());

        EConst::CpuMeshDTO dummyCpu;
        for (const auto& gpuPart : m_gpuCache[PATH]) {
            result.push_back(CreateSubMesh(dummyCpu, gpuPart, true));
        }
        return result;
    }

    std::vector<EConst::SubMesh> subMeshes = CreateCollisionSubMeshes(cpu, PATH);
    return subMeshes;
}

std::vector<EConst::SubMesh> MeshFactory::CreateCollisionSubMeshes(EConst::CpuMeshDTO cpu, std::string PATH)
{
    std::vector<EConst::SubMesh> result;

    std::vector<EConst::CpuMeshDTO> hulls = m_decomposer->runConvexDecomposition(cpu);

    std::vector<EConst::GpuMeshDTO> gpuCacheList;
    gpuCacheList.reserve(hulls.size());
    result.reserve(hulls.size());

    for (const auto& hull : hulls)
    {
        //std::cout << ">>>>> " << hull.Name << '\n';
        //for (const auto& v : hull.Vertices) {
        //    std::cout << v.pos.x << " " << v.pos.y << " " << v.pos.z << " "
        //        << v.color.x << " " << v.color.y << " " << v.color.z << " " << v.color.w << '\n';
        //}

        EConst::GpuMeshDTO gpu = CreateGPUBuffers(hull);

        gpuCacheList.push_back(gpu);

        result.push_back(CreateSubMesh(hull, gpu, false));
    }

    m_gpuCache[PATH] = gpuCacheList;

    return result;
}

EConst::SubMesh MeshFactory::CreateSubMesh(EConst::CpuMeshDTO cpu, EConst::GpuMeshDTO gpu, bool isStatic)
{
    EConst::SubMesh subMesh;

    subMesh.vBuffer = gpu.vBuffer;
    subMesh.iBuffer = gpu.iBuffer;
    subMesh.indexCount = gpu.iCount;

    subMesh.box = gpu.bounds;

    if (!isStatic) {
        subMesh.Vertices = cpu.Vertices;
        subMesh.Indices = cpu.Indices;
    }

    return subMesh;
}

EConst::GpuMeshDTO MeshFactory::CreateGPUBuffers(EConst::CpuMeshDTO hull)
{
    EConst::GpuMeshDTO gpu;

    CD3D11_BUFFER_DESC vDesc(
        static_cast<UINT>(hull.Vertices.size() * sizeof(EConst::VertexPositionColor)),
        D3D11_BIND_VERTEX_BUFFER
    );

    D3D11_SUBRESOURCE_DATA vData;
    ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
    vData.pSysMem = hull.Vertices.data();
    vData.SysMemPitch = 0;
    vData.SysMemSlicePitch = 0;

    CD3D11_BUFFER_DESC iDesc(
        static_cast<UINT>(hull.Indices.size() * sizeof(hull.Indices[0])),
        D3D11_BIND_INDEX_BUFFER
    );

    D3D11_SUBRESOURCE_DATA iData;
    ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
    iData.pSysMem = hull.Indices.data();
    iData.SysMemPitch = 0;
    iData.SysMemSlicePitch = 0;

    HRESULT hr = m_device->CreateBuffer(&vDesc, &vData, &gpu.vBuffer);
    if (FAILED(hr)) throw;

    gpu.iCount = static_cast<long>(hull.Indices.size());

    hr = m_device->CreateBuffer(&iDesc, &iData, &gpu.iBuffer);
    if (FAILED(hr)) throw;

    if (gpu.vBuffer == nullptr || gpu.iBuffer == nullptr) throw;

    gpu.Name = hull.Name;

    DirectX::BoundingBox::CreateFromPoints(
        gpu.bounds,
        static_cast<size_t>(hull.Vertices.size()),
        &hull.Vertices[0].pos,
        sizeof(EConst::VertexPositionColor)
    );

    return gpu;
}
