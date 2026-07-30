	#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <memory>
#include "EngineConstants.h"
#include "ScriptPerfab.h"

//TODO: m_children should be shared_ptr

class Node3D
{
public:
	Node3D(): m_parent(nullptr) {
		DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&m_localTransform, identity);
		DirectX::XMStoreFloat4x4(&m_globalTransform, identity);
	}

	virtual ~Node3D() {}

	virtual void Initialize();

	void ProcessPendingChanges();

	virtual void Update(float delta);

	virtual void UpdateTransforms(DirectX::FXMMATRIX parentGlobal);

	virtual void UpdateChildTransforms();

#pragma region Setters

	inline virtual void SetName(std::string newName) {
		m_name = std::move(newName);
	}
	
	virtual void SetPosition(DirectX::XMFLOAT3 newPosition);

	virtual void SetRotation(DirectX::XMFLOAT3 newRotation);

	virtual void SetScale(DirectX::XMFLOAT3 newScale);
	
	virtual inline void MarkDirty() {
		m_dirty = true;
	}

	virtual inline void ClearDirty() {
		m_dirty = false;
	}

	template <typename T>
	void AddScript() requires std::is_base_of_v<ScriptPerfab, T> {
		m_scriptsToAdd.push_back(std::make_shared<T>(this));
	}

	virtual void SetParent(Node3D* parent) {
		if (m_parent == nullptr && parent != nullptr) {
			m_parent = parent;
		}
	}

	void RemoveScript(std::shared_ptr<ScriptPerfab> script) {
		m_scriptsToRemove.push_back(script);
	}

	virtual void AddTag(std::string tag);

	virtual void RemoveTag(std::string tag);

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

	inline std::vector<Node3D*> GetChildren() const {
		return m_children;
	}

	inline Node3D* GetParent() const {
		return m_parent;
	}

	inline bool HasScripts() {
		return m_attachedScripts.size() > 0;
	}

	inline std::vector<std::string> GetTags() {
		return m_attachedTags;
	}

	inline bool HasTag(std::string tag) {
		auto it = std::find(m_attachedTags.begin(), m_attachedTags.end(), tag);
		if (it != m_attachedTags.end())
		{
			return true;
		}
		return false;
	}

#pragma endregion

#pragma region TreeTools

	virtual void addChild(Node3D* child);

	virtual void removeChild(Node3D* child);

	virtual inline bool isDirty() {
		return m_dirty;
	}
#pragma endregion

protected:

	void RebuildLocalTransform();

	//Tree
	std::string m_name = "default_name";
	Node3D* m_parent = nullptr;
	bool m_dirty = true; //If true should call UpdateTransforms for all childrens TODO:fix


	std::vector<EConst::VertexPositionColor> m_localVertices;
	std::vector<long> m_localIndices;

	//Stuff
	DirectX::XMFLOAT3 m_position = { 0,0,0 };
	DirectX::XMFLOAT3 m_rotation = { 0,0,0,};
	DirectX::XMFLOAT3 m_scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 m_localTransform;
	DirectX::XMFLOAT4X4 m_globalTransform;

	//PendingChangesArrays
	std::vector<std::shared_ptr<ScriptPerfab>> m_scriptsToAdd;
	std::vector<std::shared_ptr<ScriptPerfab>> m_scriptsToRemove;

	std::vector<Node3D*> m_childrenToAdd;
	std::vector<Node3D*> m_childrenToRemove;

	std::vector<std::string> m_tagsToAdd;
	std::vector<std::string> m_tagsToRemove;

	//======================================
	//WARNING! DO NOT CHANGE THESE DIRECTLY!
	//======================================
	std::vector<Node3D*> m_children;
	std::vector<std::shared_ptr<ScriptPerfab>> m_attachedScripts;
	std::vector<std::string> m_attachedTags;
};