#include "Node3D.h";
#pragma once
class ScriptPerfab
{
public:
	ScriptPerfab(Node3D* obj) { m_controlledNode = obj; };
	virtual void OnLoad() {};
	virtual void OnUpdate(float delta) {};
	virtual void OnChildAdded(Node3D* child) {};
	virtual void OnChildRemoved(Node3D* child) {};
	virtual ~ScriptPerfab() = default;
protected:
	Node3D* m_controlledNode;
};

