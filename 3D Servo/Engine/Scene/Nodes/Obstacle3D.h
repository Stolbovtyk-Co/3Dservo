#pragma once

#include "MeshInstance3D.h"

class Obstacle3D : public MeshInstance3D
{
public:
	Obstacle3D() : MeshInstance3D() {};

	inline void SetCollisionSubMeshes(std::vector<EConst::SubMesh> subMeshes) { m_collisionSubMeshes = std::move(subMeshes); };
	inline std::vector<EConst::SubMesh> GetCollisionSubMeshes() { return m_collisionSubMeshes; };

	inline virtual EConst::Instruction GetGPUInstruction() {
		EConst::Instruction inst;
		if (!HasTag("SV_COLLIDER_DEBUG")) {
			inst.subMeshes = m_subMeshes;
		}
		else {
			inst.subMeshes = m_collisionSubMeshes;
		}
		inst.SV_TRANSPARENT = HasTag("SV_TRANSPARENT");
		inst.world = GetGlobalTransform();
		return inst;
	}
protected:
	std::vector<EConst::SubMesh> m_collisionSubMeshes;
};

