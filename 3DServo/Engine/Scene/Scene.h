#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Importers/FileManager.h"
#include <memory>
#include <string>
#include "Nodes/Node3D.h"
#include <vector>
#include "EngineConstants.h"
#include "Resources/Mesh/MeshFactory.h"
#include "Resources/Materials/mtlManager.h"
#include "Resources/Mesh/ObjectManager.h"
#include "Core/Logger.h"

class Scene
{
public:
	
	Scene(Microsoft::WRL::ComPtr<ID3D11Device> com_device);

	 virtual void Update(float delta);

	 struct SceneSettings
	 {
		 DirectX::XMFLOAT4 clearColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		 std::string sceneName;
		 DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(0.0f, 0.0f, 1.5f, 0.f);
		 DirectX::XMVECTOR cameraLookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.f);
	 };

	 virtual ~Scene() = default;


protected:

	virtual void Setup();

	Node3D m_tree;

	std::unique_ptr<MeshFactory> m_MeshFactory;

	std::vector<std::shared_ptr<Node3D>> m_sceneObjects;

	FileManager m_fileMgr;
	std::unique_ptr<mtlManager> m_mtlManager;
	std::unique_ptr<ObjectManager> m_objectManager;

	SceneSettings m_st;

	Logger m_logger;

	//---------------------------------------
	//	IMPORTED FROM RENDER ENGINE CLASSES:
	Microsoft::WRL::ComPtr<ID3D11Device> m_com_device;

	//---------------------------------------
	//	GETTER && SETTER ZONE
public:
	virtual mtlManager* GetMtlManager() {
		return m_mtlManager.get();
	}

	SceneSettings GetSceneSettings() {
		return m_st;
	}

	inline const Node3D* GetTree() {
		return &m_tree;
	}

	virtual std::vector<EConst::Instruction> GetGPUInstructions();
};

