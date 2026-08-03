#include "ObjectManager.h"

void ObjectManager::LoadObjects(std::vector<std::string> objects)
{
	for (auto& obj : objects) {
		LoadObject(obj);
	}
}

void ObjectManager::LoadObject(std::string object)
{
	if (std::find(m_knownObjectFiles.begin(), m_knownObjectFiles.end(), object) == m_knownObjectFiles.end()) {
		auto lines = m_flMgr->ReadText(object);
		m_mtlMgr->AddNewMtlFiles(m_objParser.GetRequiredMtlFiles(lines));
		std::vector<EConst::CpuMesh> meshes =  m_objParser.TxtToObj(lines);
		std::vector<EConst::SubMesh> buffers;
		for (auto& mesh : meshes) {
			buffers.push_back(LoadObjectToGPU(mesh));
		}
        LOG_INFO(m_logger, "New object loaded: " + object);
		m_objectDB.emplace(object, buffers);

		m_knownObjectFiles.push_back(object);
	}
}

const std::vector<EConst::SubMesh>& ObjectManager::GetObjectSubMeshes(std::string name, bool autoLoad)
{
	auto mat = m_objectDB.find(name);
	if (mat != m_objectDB.end()) {
		return mat->second;
	}
	else {
		if (autoLoad) {
			LoadObject(name);
			return GetObjectSubMeshes(name, false);
		}
		else {
			throw std::out_of_range("Object not found or failed to load");
		}
	}
}

EConst::SubMesh ObjectManager::LoadObjectToGPU(const EConst::CpuMesh& mesh)
{
    EConst::SubMesh out;

    // Vertex Buffer
    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.ByteWidth = static_cast<UINT>(
        mesh.Vertices.size() * sizeof(EConst::VertexInfo));
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = mesh.Vertices.data();

    HRESULT hr = m_comDevice->CreateBuffer(
        &vbDesc,
        &vbData,
        out.vBuffer.GetAddressOf());

    if (FAILED(hr))
        throw std::runtime_error("Failed to create vertex buffer.");

    // Index Buffer
    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.ByteWidth = static_cast<UINT>(
        mesh.Indices.size() * sizeof(uint32_t));
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = mesh.Indices.data();

    hr = m_comDevice->CreateBuffer(
        &ibDesc,
        &ibData,
        out.iBuffer.GetAddressOf());

    if (FAILED(hr))
        throw std::runtime_error("Failed to create index buffer.");

    // Metadata
    out.indexCount = static_cast<uint32_t>(mesh.Indices.size());
    out.MaterialRegions = mesh.SubMeshes;

    // Bounding Box
    if (!mesh.Vertices.empty())
    {
        std::vector<DirectX::XMFLOAT3> positions;
        positions.reserve(mesh.Vertices.size());

        for (const auto& v : mesh.Vertices)
            positions.push_back(v.pos);

        DirectX::BoundingBox::CreateFromPoints(
            out.box,
            positions.size(),
            positions.data(),
            sizeof(DirectX::XMFLOAT3));
    }

    return out;
}
