#include "Node3D.h"
	using namespace DirectX;

void Node3D::SetOffset(DirectX::XMFLOAT3 newOffset) {
	XMFLOAT3 vectorResult;

	XMVECTOR GlobalPos = XMLoadFloat3(&m_data.GlobalPos);
	XMVECTOR vB = XMLoadFloat3(&m_offset);
	XMVECTOR vC = XMLoadFloat3(&newOffset);

	GlobalPos = GlobalPos - vB;
	m_offset = newOffset;
	GlobalPos = GlobalPos + vC;

	XMStoreFloat3(&vectorResult, GlobalPos);

	m_data.GlobalPos = vectorResult;
	
	UpdateGVertices(GlobalPos);

	for (auto* ch : m_children)
	{
		ch->ChangeGPos(GlobalPos);
	}
}

void Node3D::UpdateGVertices(DirectX::XMVECTOR newGPos) {
	if (m_localVertices.begin() == m_localVertices.end()) { //TODO: Check Later
		return;
	}

	m_data.GlobalVertices.clear();

	for (auto vr : m_localVertices)
	{
		XMFLOAT3 Result;
		XMVECTOR vertice = XMLoadFloat3(&vr);

		XMStoreFloat3(&Result, vertice += newGPos);
		m_data.GlobalVertices.emplace_back(Result);
	}
}

void Node3D::ChangeGPos(DirectX::XMVECTOR newParentGPos) {
	XMFLOAT3 vectorResult;

	XMVECTOR vA = XMLoadFloat3(&m_offset);

	XMVECTOR vResult = vA + newParentGPos;

	XMStoreFloat3(&vectorResult, vResult);

	m_data.GlobalPos = vectorResult;
	
	UpdateGVertices(vResult);
	
	for (auto* ch : m_children)
	{
		ch->ChangeGPos(vResult);
	}
}