#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <list>
#include "EngineConstants.h"
#include "FileManager.h"

class Node3D
{
public:
	Node3D(): m_parent(nullptr) {
		DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&m_localTransform, identity);
		DirectX::XMStoreFloat4x4(&m_globalTransform, identity);
	}

	virtual ~Node3D() {
		if (m_pVertexBuffer) m_pVertexBuffer->Release();
		if (m_pIndexBuffer) m_pIndexBuffer->Release();
	}

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void UpdateTransforms(DirectX::FXMMATRIX parentGlobal);

	virtual void SetObjFile(std::string FILE, FileManager* flMgr, Microsoft::WRL::ComPtr<ID3D11Device> com_device);

#pragma region Setters

	inline virtual void SetName(std::string newName) {
		m_name = std::move(newName);
	}

	
	virtual void SetPosition(DirectX::XMFLOAT3 newPosition);

	virtual void SetRotation(DirectX::XMFLOAT3 newRotation);

	virtual void SetScale(DirectX::XMFLOAT3 newScale);
	

	inline virtual void SetLocalIndeces(std::vector<long> newInd) {
		m_localIndices = std::move(newInd);
	}

	inline virtual void SetLocalVertices(std::vector<EConst::VertexPositionColor> newVert) {
		m_localVertices = std::move(newVert);
	}
	

	inline void SetGPUBuffers(ID3D11Buffer* vBuffer, ID3D11Buffer* iBuffer, long indexCount) {
		m_pVertexBuffer = vBuffer;
		m_pIndexBuffer = iBuffer;
		m_gpuIndexCount = indexCount;
	}
#pragma endregion

#pragma region Getters

	inline virtual std::string GetName() const {
		return m_name;
	}

	inline virtual DirectX::XMFLOAT3 getLocalPosition() const {
		return m_position;
	}

	inline virtual DirectX::XMFLOAT3 getLocalRotation() const {
		return m_rotation;
	}

	inline virtual DirectX::XMFLOAT3 getLocalScale() const {
		return m_scale;
	}

	inline virtual DirectX::XMFLOAT4X4 GetGlobalTransform() const {
		return m_globalTransform;
	}

	inline const std::vector<EConst::VertexPositionColor>& GetVertexPositionColor() const {
		return m_localVertices;
	}

	inline std::vector<long> GetIndeces() const {
		return m_localIndices;
	}

	inline long GetIndexCount() const {
		return static_cast<long>(m_localIndices.size());
	}

	inline long GetGPUIndexCount() const {
		return m_gpuIndexCount;
	}

	inline ID3D11Buffer* GetVertexBuffer() const { return m_pVertexBuffer; }
	
	inline ID3D11Buffer* GetIndexBuffer() const { return m_pIndexBuffer; }
	
	inline bool HasGeometry() const {
		return m_pVertexBuffer != nullptr && m_pIndexBuffer != nullptr;
	}

	inline std::vector<Node3D*> GetChildren() const {
		return m_children;
	}
#pragma endregion

#pragma region TreeTools

	virtual void addChild(Node3D* child);

	virtual void removeChild(Node3D* child);

#pragma endregion

protected:

	void RebuildLocalTransform();

	//Tree
	long m_gpuIndexCount = 0;
	std::string m_name = "default_name";
	Node3D* m_parent = nullptr;
	std::vector<Node3D*> m_children;

	std::vector<EConst::VertexPositionColor> m_localVertices;
	std::vector<long> m_localIndices;

	//Stuff
	DirectX::XMFLOAT3 m_position = { 0,0,0 };
	DirectX::XMFLOAT3 m_rotation = { 0,0,0,};
	DirectX::XMFLOAT3 m_scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 m_localTransform;

	//VideoCar Buffers
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	DirectX::XMFLOAT4X4 m_globalTransform;

	//further on comes some abstruse crap 
};