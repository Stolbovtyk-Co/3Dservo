#include "Node3D.h"

void Node3D::Initialize()
{
	for (auto& sc : m_attachedScripts) {
		if (sc) {
			sc->OnLoad();
		}
	}
	for (auto ch : m_children) {
		ch->Initialize();
	}
}

void Node3D::Update(float delta)
{
	for (auto& sc : m_attachedScripts) {
		if (sc) {
			sc->OnUpdate(delta);
		}
	}
	for (auto ch : m_children) { // TODO: rework update calls
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
	m_attachedTags.push_back(tag);
	for (auto& sc : m_attachedScripts) {
		if (sc) {
			sc->OnTagAdded(tag);
		}
	}
}

void Node3D::RemoveTag(std::string tag)
{
	auto it = std::find(m_attachedTags.begin(), m_attachedTags.end(), tag);
	if (it != m_attachedTags.end())
	{
		for (auto& sc : m_attachedScripts) {
			if (sc) {
				sc->OnTagRemoved(tag);
			}
		}
		m_attachedTags.erase(it);
	}
}

void Node3D::RebuildLocalTransform() {
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	DirectX::XMMATRIX local = scaleMat * rotMat * transMat;
	XMStoreFloat4x4(&m_localTransform, local);


	DirectX::XMMATRIX parentGlobal = m_parent ? XMLoadFloat4x4(&m_parent->m_globalTransform) : DirectX::XMMatrixIdentity();
	MarkDirty();
}

void Node3D::addChild(Node3D* child)
{
	if (!child) return;

	child->m_parent = this;
	m_children.push_back(child);

	for (auto& sc : m_attachedScripts) {
		if (sc) {
			sc->OnChildAdded(child);
		}
	}

	MarkDirty();
}

void Node3D::removeChild(Node3D* child)
{
	auto it = std::find(m_children.begin(), m_children.end(), child);
	if (it != m_children.end())
	{
		for (auto& sc : m_attachedScripts) {
			if (sc) {
				sc->OnChildRemoved(child);
			}
		}
		(*it)->m_parent = nullptr;
		m_children.erase(it);
	}
}
