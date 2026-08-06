#pragma once
#include <memory>
#include <string>
#include "Scene/Nodes/Node3D.h"
#include "ObjectManager.h"

class Logger;
class FileManager;
class MeshInstance3D;

class MeshFactory
{
public:
	MeshFactory(ObjectManager* objMgr) 
	{
		m_objMgr = objMgr;
	}

	~MeshFactory() = default;

	std::shared_ptr<Node3D> CreateStaticInstance(std::string PATH);

private:
	ObjectManager* m_objMgr;
};

