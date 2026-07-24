#include "Scene.h"

Scene::Scene()
{
	Setup();
}

void Scene::Setup()
{
	// Load Scene Tree

	// SETUP SHADER ON CHILD LEVEL
}

void Scene::Update(float delta)
{
}

Scene::ShaderManager::ShaderManager()
{
	Scene::ShaderManager::Setup();
}

Scene::ShaderManager::ShaderDTO Scene::ShaderManager::GetShaderManagerDTO()
{
	Scene::ShaderManager::ShaderDTO dto;
	dto.constantBuffer = Scene::ShaderManager::m_constantBuffer;
	dto.inputLayout = Scene::ShaderManager::m_inputLayout;
	dto.pixelShader = Scene::ShaderManager::m_pixelShader;
	dto.vertexShader = Scene::ShaderManager::m_vertexShader;
	return dto;
}

void Scene::ShaderManager::Setup()
{
}
