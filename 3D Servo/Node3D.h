#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <list>
#include "EngineConstants.h"

class Node3D
{
public:
	Node3D(long id) : m_ID(id), m_parent(nullptr) {
		DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&m_localTransform, identity);
		DirectX::XMStoreFloat4x4(&m_globalTransform, identity);
	}

	virtual void Initialize();

	virtual void Update();

	virtual void UpdateTransforms(DirectX::FXMMATRIX parentGlobal);

#pragma region Setters

	inline virtual void SetName(std::string newName) {
		m_name = std::move(newName);
	}

	virtual void SetPosition(DirectX::XMFLOAT3 newPos);

	inline virtual void SetLocalIndeces(std::vector<long> newInd) {
		m_localIndices = std::move(newInd);
	}

	inline virtual void SetLocalVertices(std::vector<EConst::VertexPositionColor> newVert) {
		m_localVertices = std::move(newVert);
	}
		
#pragma endregion

#pragma region Getters

	inline virtual std::string GetName() const {
		return m_name;
	}

	inline virtual DirectX::XMFLOAT3 getLocalPosition() const {
		return DirectX::XMFLOAT3(m_localTransform._41, m_localTransform._42, m_localTransform._43);
	}

	inline virtual DirectX::XMFLOAT4X4 GetGlobalTransform() const {
		return m_globalTransform;
	}

	inline const std::vector<EConst::VertexPositionColor>& GetVertexPositionColor() const {
		return m_localVertices;
	}

	inline std::vector<long> GetIndexBuffer() const {
		return m_localIndices;
	}

	inline long GetIndexCount() const {
		return static_cast<long>(m_localIndices.size());
	}

#pragma endregion

#pragma region TreeTools

	virtual void addChild(Node3D* child);

	virtual void removeChild(Node3D* child);

#pragma endregion

protected:

	//Tree
	long m_ID;
	std::string m_name = "default_name";
	Node3D* m_parent = nullptr;
	std::vector<Node3D*> m_children;	//3D

	std::vector<EConst::VertexPositionColor> m_localVertices;
	std::vector<long> m_localIndices;

	DirectX::XMFLOAT4X4 m_localTransform;
	DirectX::XMFLOAT4X4 m_globalTransform;

	//further on comes some abstruse crap 
};