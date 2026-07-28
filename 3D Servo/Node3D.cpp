#include "Node3D.h"

void Node3D::Initialize()
{
	for (auto ch : m_children) {
		ch->Initialize();
	}
}

void Node3D::ProcessPendingChanges()
{
	// Remove scripts
	for (const auto& script : m_scriptsToRemove) {
		auto it = std::find(m_attachedScripts.begin(), m_attachedScripts.end(), script);
		if (it != m_attachedScripts.end()) {
			m_attachedScripts.erase(it);
		}
	}
	m_scriptsToRemove.clear();
	//Add scripts
	for (const auto& script : m_scriptsToAdd) {
		m_attachedScripts.push_back(script);
		script->OnLoad();
	}
	m_scriptsToAdd.clear();
	//Remove children
	for (const auto& child : m_childrenToRemove) {
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end()) {
			// Call event
			for (auto& sc : m_attachedScripts) {
				if (sc) {
					sc->OnChildRemoved(child);
				}
			}
			m_children.erase(it);
		}
	}
	m_childrenToRemove.clear();
	//Add children
	for (const auto& child : m_childrenToAdd) {
		m_children.push_back(child);
		child->ProcessPendingChanges();
		child->UpdateTransforms(DirectX::XMLoadFloat4x4(&m_globalTransform));
		// Call event
		for (auto& sc : m_attachedScripts) {
			if (sc) {
				sc->OnChildAdded(child);
			}
		}
	}
	if (m_dirty) {
		UpdateTransforms(DirectX::XMLoadFloat4x4(&(m_parent->m_globalTransform)));
		ClearDirty();
	}
	m_childrenToAdd.clear();
	//Remove tags
	for (const auto& tag : m_tagsToRemove) {
		auto it = std::find(m_attachedTags.begin(), m_attachedTags.end(), tag);
		if (it != m_tagsToRemove.end()) {
			// Call event
			for (auto& sc : m_attachedScripts) {
				if (sc) {
					sc->OnTagRemoved(tag);
				}
			}
			m_attachedTags.erase(it);
		}
	}
	m_tagsToRemove.clear();
	//Add tags
	for (const auto& tag : m_tagsToAdd) {
		m_attachedTags.push_back(tag);
		//Call event
		for (auto& sc : m_attachedScripts) {
			if (sc) {
				sc->OnTagAdded(tag);
			}
		}
	}
	m_tagsToAdd.clear();
	//Call ProcessPendingChanges() in children;
	for (auto& child : m_children) {
		if (child) {
			child->ProcessPendingChanges();
		}
	}
}

void Node3D::Update(float delta)
{
	for (auto& sc : m_attachedScripts) {
		if (sc) {
			sc->OnUpdate(delta);
		}
	}
	for (auto ch : m_children) { 
		ch->Update(delta);
	}
}

void Node3D::UpdateTransforms(DirectX::FXMMATRIX parentGlobalTransform) //TODO: Set call in Scene.cpp
{
	DirectX::XMMATRIX local = DirectX::XMLoadFloat4x4(&m_localTransform);
	DirectX::XMMATRIX global = local * parentGlobalTransform;
	DirectX::XMStoreFloat4x4(&m_globalTransform, global);

	for (auto child : m_children) {
		child->UpdateTransforms(global);
	}
}

void Node3D::SetPosition(DirectX::XMFLOAT3 newPos)
{
	m_position = std::move(newPos);
	RebuildLocalTransform();
}

void Node3D::SetRotation(DirectX::XMFLOAT3 newRotation)
{
	m_rotation = std::move(newRotation);
	RebuildLocalTransform();
}

void Node3D::SetScale(DirectX::XMFLOAT3 newScale)
{
	m_scale = std::move(newScale);
	RebuildLocalTransform();
}

void Node3D::AddTag(std::string tag)
{
	m_tagsToAdd.push_back(tag);
}

void Node3D::RemoveTag(std::string tag)
{
	m_tagsToRemove.push_back(tag);
}

void Node3D::RebuildLocalTransform() {
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	DirectX::XMMATRIX local = scaleMat * rotMat * transMat;
	XMStoreFloat4x4(&m_localTransform, local);

	MarkDirty();
}

void Node3D::addChild(Node3D* child)
{
	if (!child) return;

	m_childrenToAdd.push_back(child);
}

void Node3D::removeChild(Node3D* child)
{
	if (!child) return;

	m_childrenToRemove.push_back(child);
}
