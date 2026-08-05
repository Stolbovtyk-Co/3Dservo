#include "Scene.h"
#include "EngineConstants.h"
#include "Env/RotateScript.h" //TODO: remove
#include "Resources/Mesh/MeshFactory.h"
#include "Scene/Nodes/MeshInstance3D.h"

Scene::Scene(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	m_com_device = device;
	Setup();
}

void Scene::Setup()
{
	m_st.clearColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_st.sceneName = "gm_Scene_Perfab";

	m_mtlManager = std::make_unique<mtlManager>(&m_fileMgr, m_com_device);
	m_objectManager = std::make_unique<ObjectManager>(&m_fileMgr,m_mtlManager.get(), m_com_device);

	m_MeshFactory = std::make_unique<MeshFactory>(m_objectManager.get());
	m_tree.Initialize();

	std::shared_ptr<Node3D> sceneMesh = m_MeshFactory->CreateStaticInstance("SceneCR.obj");
	std::size_t i = m_sceneObjects.size();
	m_sceneObjects.push_back(sceneMesh);
	//m_sceneObjects[i]->AddTag("SV_TRANSPARENT");
	m_sceneObjects[i]->AddScript<RotateScript>();
	m_sceneObjects[i]->SetPosition({ 0.0f, 0.0f, -4.0f });
	m_sceneObjects[i]->SetScale({ 3.f, 3.f, 3.f });
	m_tree.addChild(m_sceneObjects[i]);

	/* std::shared_ptr<Node3D> sceneMesh2 = m_MeshFactory->CreateStaticInstance("SceneCR.obj");
	i = m_sceneObjects.size();
	m_sceneObjects.push_back(sceneMesh2);
	m_sceneObjects[i]->AddTag("SV_TRANSPARENT");
	m_sceneObjects[i]->AddScript<RotateScript>();
	m_sceneObjects[i]->SetPosition({1.0f, 0.0f, 0.0f});
	m_tree.addChild(m_sceneObjects[i]); */
}

void Scene::Update(float delta)
{
	m_tree.ProcessPendingChanges();
	m_tree.Update(delta);
}

std::vector<EConst::Instruction> Scene::GetGPUInstructions()
{
	std::vector<EConst::Instruction> insts;
	for (auto& node : m_sceneObjects) {
		if (node->GetParent() != nullptr) {
			if (auto meshNode = std::dynamic_pointer_cast<MeshInstance3D>(node)) {
				if (meshNode->HasSubMeshes() && !meshNode->HasTag("SV_HIDE")) {
					insts.push_back(meshNode->GetGPUInstruction());
				}
			}
		}
	}
	return insts;
}
