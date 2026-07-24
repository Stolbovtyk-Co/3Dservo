#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <list>

class Node3D
{
public:
	Node3D(long id) {long m_ID = id; }

	virtual void Initialize() { }

	virtual void Update();

	virtual void ChangeGPos(DirectX::XMVECTOR newParentGPos);

#pragma region Getters

	inline virtual std::string GetName() {
		return m_name;
	}

	inline virtual DirectX::XMFLOAT3 GetOffset() {
		return m_offset;
	}

	inline virtual 
#pragma endregion

#pragma region Setters

	inline virtual void SetName(std::string newName) {
		m_name = newName;
	}

	inline virtual void SetOffset(DirectX::XMFLOAT3 newPos);
		
#pragma endregion

#pragma region TreeTools

	inline virtual void addChild(Node3D* child) {

		m_children.push_back(child);
	}

	inline virtual void removeChild(Node3D* child) {
		m_children.remove(child);
	}

#pragma endregion
	

	void UpdateGVertices(DirectX::XMVECTOR newGPos);

protected:

	struct RenderDTO //Только брать - ничего не класть!!
	{
		DirectX::XMFLOAT3 GlobalPos;
		std::vector<DirectX::XMFLOAT3> GlobalVertices;
		std::vector<DirectX::XMFLOAT3> Normals;
	};

	//Tree
	long m_ID;
	std::string m_name = "default_name";
	std::list<Node3D*> m_children; //( ´･･)ﾉ (._.`) Because vitolde100!! List for remove!
	//3D
	RenderDTO m_data;

	std::vector<DirectX::XMFLOAT3> m_localVertices;

	DirectX::XMFLOAT3 m_offset;
	//further on comes some abstruse crap 
	DirectX::XMFLOAT2 m_textureCoordinates;
	
};