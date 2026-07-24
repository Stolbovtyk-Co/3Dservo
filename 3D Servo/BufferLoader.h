#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

class BufferLoader
{
public:
	void LoadBuffer(Microsoft::WRL::ComPtr<ID3D11Device> com_device);

	struct BuffersStaffDTO {
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	};
	
	inline BuffersStaffDTO GetBuffersStaff() {
		BuffersStaffDTO dto;
		dto.VertexBuffer = m_vertexBuffer;
		dto.IndexBuffer = m_indexBuffer;
		return dto;
	}

	inline long GetIndexCount() {
		return m_indexCount;
	}

	typedef struct _vertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	} VertexPositionColor;

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	long m_indexCount;

};

