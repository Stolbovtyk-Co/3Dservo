#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <string>

namespace EConst {
	typedef struct _vertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
	} VertexPositionColor;

	typedef struct _constantBufferStruct {
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	} ConstantBufferStruct;

	 struct RawMeshDTO {
		std::string Name = "default_name";
		std::vector<float> Vertices;
		std::vector<short> Indices;
	};

	 struct CpuMeshDTO {
		std::string Name = "default_name";
		std::vector<EConst::VertexPositionColor> Vertices;
		std::vector<short> Indices;
	};

	struct GpuMeshDTO
	{
		std::string Name;
		Microsoft::WRL::ComPtr < ID3D11Buffer> vBuffer;
		Microsoft::WRL::ComPtr < ID3D11Buffer> iBuffer;
		long iCount = 0;
		DirectX::BoundingBox bounds;
	};

	struct SubMesh {
		//Render
		Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer;
		long indexCount = 0;

		//Dynamic Mesh
		std::vector<EConst::VertexPositionColor> Vertices;
		std::vector<short> Indices;

		//For Queue
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
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
		long indexCount = 0;
		float cDistSqr = 0;
	};
	struct GPUIstructionsDTO {
		std::vector<GPUBuffers> regular;
		std::vector<GPUBuffers> transparent;
	};
}
