#pragma once
#include "RotateScript.h"
#include "ScriptPerfab.h";

#include "Node3D.h";

class RotateScript : public ScriptPerfab
{
public:
	RotateScript(Node3D* obj) : ScriptPerfab(obj) {};

	void OnUpdate(float delta) override {
		float rotSp = 0.005;
		auto r = DirectX::XMFLOAT3(m_controlledNode->getLocalRotation().x + rotSp,
			m_controlledNode->getLocalRotation().y + rotSp, m_controlledNode->getLocalRotation().z + rotSp / 2);
		m_controlledNode->SetRotation(r);
	}
};

