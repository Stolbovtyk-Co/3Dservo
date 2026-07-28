#include "MeshFactory.h"
#include <DirectXCollision.h>

#include "ObjParser.h"
#include "FileManager.h"

#include "MeshInstance3D.h"
#include "ConvexDecomposer.h"

std::shared_ptr<Node3D> MeshFactory::CreateStaticInstance(std::string PATH)
{
    //TODO: ASYNCH HERE!!! 
    std::vector<EConst::SubMesh> hulls = GetStaticSubMeshes(PATH);

    std::shared_ptr<Node3D> newInstance = std::make_shared<MeshInstance3D>(hulls);
    return newInstance;
}

std::vector<EConst::SubMesh> MeshFactory::GetStaticSubMeshes(std::string PATH)
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

    auto rawData = m_fileManager->ReadText(PATH);
    auto cpu = ObjParser::ParseLines(rawData);

    std::vector<EConst::SubMesh> subMeshes = CreateSubMeshes(cpu, PATH);
    return subMeshes;
}

std::vector<EConst::SubMesh> MeshFactory::CreateSubMeshes(EConst::CpuMeshDTO cpu, std::string PATH)
{
    std::vector<EConst::SubMesh> result;

    std::vector<EConst::CpuMeshDTO> hulls = m_decomposer->runConvexDecomposition(cpu);

    std::vector<EConst::GpuMeshDTO> gpuCacheList;
    gpuCacheList.reserve(hulls.size());
    result.reserve(hulls.size());

    for (const auto& hull : hulls)
    {
        EConst::GpuMeshDTO gpu;

        CD3D11_BUFFER_DESC vDesc(
            hull.Vertices.size() * sizeof(EConst::VertexPositionColor),
            D3D11_BIND_VERTEX_BUFFER
        );

        D3D11_SUBRESOURCE_DATA vData;
        ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
        vData.pSysMem = hull.Vertices.data();
        vData.SysMemPitch = 0;
        vData.SysMemSlicePitch = 0;

        CD3D11_BUFFER_DESC iDesc(
            hull.Indices.size() * sizeof(short),
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
