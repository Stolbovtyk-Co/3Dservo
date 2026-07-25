#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

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
}
