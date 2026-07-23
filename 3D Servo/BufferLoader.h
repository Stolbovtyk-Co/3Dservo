#pragma once
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

class BufferLoader
{
	public:
		void LoadBuffer(Microsoft::WRL::ComPtr<ID3D11Device> com_device);
private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	long m_indexCount;

	typedef struct _vertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	} VertexPositionColor;
};

