#include "Node3D.h"

void Node3D::UpdateTransforms(DirectX::FXMMATRIX parentGlobalTransform)
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

void Node3D::RebuildLocalTransform() {
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	DirectX::XMMATRIX local = scaleMat * rotMat * transMat;
	XMStoreFloat4x4(&m_localTransform, local);


	DirectX::XMMATRIX parentGlobal = m_parent ? XMLoadFloat4x4(&m_parent->m_globalTransform) : DirectX::XMMatrixIdentity();
	UpdateTransforms(parentGlobal);
}


void Node3D::addChild(Node3D* child)
{
	if (!child) return;

	child->m_parent = this;
	m_children.push_back(child);

	child->UpdateTransforms(XMLoadFloat4x4(&m_globalTransform));
}

void Node3D::removeChild(Node3D* child)
{
	auto it = std::find(m_children.begin(), m_children.end(), child);
	if (it != m_children.end())
	{
		(*it)->m_parent = nullptr;
		m_children.erase(it);
	}
}
