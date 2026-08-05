#pragma once
#include <memory>
#include "Scene/Scene.h"
#include "EngineConstants.h"

class RenderQueueManager
{
public:
	RenderQueueManager(std::shared_ptr<Scene> sc, EConst::ConstantBufferStruct* BStruct) {
		m_scenePtr = sc;
		m_bufferStruct = BStruct;
	}
	EConst::GPUIstructionsDTO MakeRenderQueue();
private:
	std::shared_ptr<Scene> m_scenePtr;
	EConst::ConstantBufferStruct* m_bufferStruct;
};

