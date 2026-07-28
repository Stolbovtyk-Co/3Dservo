#pragma once
#include "Node3D.h"
#include "EngineConstants.h"

class MeshInstance3D : public Node3D
{
public:
	MeshInstance3D() : Node3D() { }

	MeshInstance3D(EConst::GpuMeshDTO Mesh) : Node3D() {
		SetName(Mesh.Name);
		SetGPUBuffers(Mesh.vBuffer, Mesh.iBuffer, Mesh.iCount);
	}

#pragma region Setters
	inline virtual void SetLocalIndeces(std::vector<long> newInd) {
		m_localIndices = std::move(newInd);
	}

	inline virtual void SetLocalVertices(std::vector<EConst::VertexPositionColor> newVert) {
		m_localVertices = std::move(newVert);
	}

	inline void SetGPUBuffers(Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer, long indexCount) {
		m_pVertexBuffer = vBuffer;
		m_pIndexBuffer = iBuffer;
		m_gpuIndexCount = indexCount;
	}
#pragma endregion

#pragma region Getters
	inline const std::vector<EConst::VertexPositionColor>& GetVertexPositionColor() const {
		return m_localVertices;
	}

	inline std::vector<long> GetIndeces() const {
		return m_localIndices;
	}

	inline long GetIndexCount() const {
		return static_cast<long>(m_localIndices.size());
	}

	inline long GetGPUIndexCount() const {
		return m_gpuIndexCount;
	}

	inline const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const {
		return m_pVertexBuffer;
	}

	inline const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndexBuffer() const {
		return m_pIndexBuffer;
	}

	inline bool HasGeometry() const {
		return m_pVertexBuffer != nullptr && m_pIndexBuffer != nullptr;
	}

	inline bool HasSubMeshes() { return m_subMeshes.size() > 0; }

	inline EConst::Instruction GetGPUInstruction() {
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

