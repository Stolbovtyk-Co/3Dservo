#pragma once
#include <string>

class Node3D;

class ScriptPerfab
{
public:
	ScriptPerfab(Node3D* obj) { m_controlledNode = obj;};
	virtual void OnLoad() {};
	virtual void OnUpdate(float delta) {};
	virtual void OnChildAdded(std::shared_ptr<Node3D> child) {};
	virtual void OnChildRemoved(std::shared_ptr<Node3D> child) {};
	virtual void OnTagAdded(std::string tag) {};
	virtual void OnTagRemoved(std::string tag) {};
	virtual ~ScriptPerfab() = default;
protected:
	Node3D* m_controlledNode;
};


