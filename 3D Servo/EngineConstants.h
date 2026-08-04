#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <string>
#include <vector>

namespace EConst {
	typedef struct _vertexInfo 
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
	} VertexInfo;

	typedef struct _constantBufferStruct {
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	} ConstantBufferStruct;

	struct CpuSubMesh
	{
		std::string MaterialName;
		uint32_t StartIndex;
		uint32_t IndexCount;
	};

	struct CpuMesh
	{
		std::string OName;

		std::vector<EConst::VertexInfo> Vertices;
		std::vector<uint32_t> Indices;

		std::vector<CpuSubMesh> SubMeshes;
	};

	struct SubMesh {
		//Render
		Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer;
		uint32_t indexCount = 0;

		std::vector<CpuSubMesh> MaterialRegions;

		DirectX::BoundingBox box;
	}; 

	struct Instruction {
		bool SV_TRANSPARENT = false;
		std::vector<SubMesh> subMeshes;
		DirectX::XMFLOAT4X4 world = {};
	};

	struct GPUBuffers {
		Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer;
		std::vector<CpuSubMesh> MaterialRegions;
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
		long indexCount = 0;
		float cDistSqr = 0;
	};
	struct GPUIstructionsDTO { 
		std::vector<GPUBuffers> regular;
		std::vector<GPUBuffers> transparent;
	};
	struct alignas(16) MaterialBuffer {
		DirectX::XMFLOAT3 ambientColor;  
		float padding1;                  
		DirectX::XMFLOAT3 diffuseColor;  
		float padding2;
		DirectX::XMFLOAT3 specularColor; 
		float shininess;     
		int    hasDiffuseMap = 0;
		int    hasSpecularMap = 0;
		int    hasNormalMap = 0;
		float  padding3;
	};
	struct MaterialData {
		std::string name;
		MaterialBuffer Settings;
		std::string DiffuseTextureFile;
		std::string SpecularTextureFile;
		std::string NormalTextureFile;
	};
	struct DX11Material {
		MaterialBuffer mBuffer;
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> Texture = nullptr; //map_Kd
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> SpecularMap = nullptr; //map_Ks
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> NormalMap = nullptr; //map_Bump
	};
}
