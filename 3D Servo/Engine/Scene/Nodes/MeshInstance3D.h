#pragma once
#include "Node3D.h"
#include "EngineConstants.h"

class MeshInstance3D : public Node3D
{
public:
	MeshInstance3D() : Node3D() { }

#pragma region Setters
	inline void SetSubMeshes(std::vector<EConst::SubMesh> subMeshes) {
		m_subMeshes = std::move(subMeshes);
	}
#pragma endregion

#pragma region Getters
	inline bool HasGeometry() const {
		return m_pVertexBuffer != nullptr && m_pIndexBuffer != nullptr;
	}

	inline bool HasSubMeshes() { return m_subMeshes.size() > 0; }

	inline virtual EConst::Instruction GetGPUInstruction() {
		EConst::Instruction inst;
		inst.subMeshes = m_subMeshes;
		inst.SV_TRANSPARENT = HasTag("SV_TRANSPARENT");
		inst.world = GetGlobalTransform();
		return inst;
	}

#pragma endregion

protected:
	long m_gpuIndexCount = 0;
	//VideoCar Mesh
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer = nullptr;

	std::vector<EConst::SubMesh> m_subMeshes;
};
