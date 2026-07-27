#include "Node3D.h";
#pragma once
class ScriptPerfab
{
public:
	ScriptPerfab(Node3D* obj) { m_controlledNode = obj; };
	virtual void OnLoad(float delta);
	virtual void OnUpdate(float delta);
	virtual ~ScriptPerfab() = default;
protected:
	Node3D* m_controlledNode;
};

