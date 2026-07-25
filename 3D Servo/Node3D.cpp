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
	DirectX::XMMATRIX Translation = DirectX::XMMatrixTranslation(newPos.x, newPos.y, newPos.z);
	DirectX::XMStoreFloat4x4(&m_localTransform, Translation);

	DirectX::XMMATRIX parentGlobal = m_parent ? DirectX::XMLoadFloat4x4(&m_parent->m_globalTransform) : DirectX::XMMatrixIdentity();
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
	
