#include "MeshFactory.h"
#include "ObjParser.h"
#include "FileManager.h"

#include "MeshInstance3D.h"

std::shared_ptr<Node3D> MeshFactory::CreateStaticInstance(std::string PATH)
{
    EConst::GpuMeshDTO Mesh = GetStaticMesh(PATH);

    if (Mesh.vBuffer == nullptr || Mesh.iBuffer == nullptr) {
        //m_logger->log("God damm"); currently nullptr
        throw;
    }

    std::shared_ptr<Node3D> newInstance = std::make_shared<MeshInstance3D>(Mesh);
    return newInstance;
}

EConst::GpuMeshDTO MeshFactory::GetStaticMesh(std::string PATH)
{
    if (m_gpuCache.count(PATH) > 0) return m_gpuCache[PATH];

    auto rawData = m_fileManager->ReadText(PATH);
    auto dto = ObjParser::ParseLines(rawData);

    EConst::GpuMeshDTO gpuBuffers = CreateD3DBuffers(dto);
    m_gpuCache[PATH] = gpuBuffers;
    return gpuBuffers;
}

EConst::GpuMeshDTO MeshFactory::CreateD3DBuffers(EConst::CpuMeshDTO dto)
{
    EConst::GpuMeshDTO result;

    CD3D11_BUFFER_DESC vDesc(
        dto.Vertices.size() * sizeof(EConst::VertexPositionColor),
        D3D11_BIND_VERTEX_BUFFER
    );

    D3D11_SUBRESOURCE_DATA vData;
    ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
    vData.pSysMem =  dto.Vertices.data();
    vData.SysMemPitch = 0;
    vData.SysMemSlicePitch = 0;

    CD3D11_BUFFER_DESC iDesc(
        dto.Indices.size() * sizeof(short),
        D3D11_BIND_INDEX_BUFFER
    );

    D3D11_SUBRESOURCE_DATA iData;
    ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
    iData.pSysMem = dto.Indices.data();
    iData.SysMemPitch = 0;
    iData.SysMemSlicePitch = 0;

    HRESULT hr = m_device->CreateBuffer(
        &vDesc,
        &vData,
        &result.vBuffer
    );
    if (FAILED(hr)) {
        //m_logger->logHR(hr);
        throw;
    }

    result.iCount = static_cast<long>(dto.Indices.size());

    hr = m_device->CreateBuffer(
        &iDesc,
        &iData,
        &result.iBuffer
    );
    if (FAILED(hr)) {
        //m_logger->logHR(hr);
        throw;
    }

    result.Name = dto.Name;

    return result;
}

