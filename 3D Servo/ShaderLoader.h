#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

class ShaderLoader
{
public:
	struct ShadersStuffDTO {
		Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	};

	typedef struct _constantBufferStruct {
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	} ConstantBufferStruct;
	
	void LoadShaders(Microsoft::WRL::ComPtr<ID3D11Device> com_device);

	inline ShadersStuffDTO GetShadersStuff() {
		ShadersStuffDTO dto;
		dto.VertexShader = m_vertexShader;
		dto.InputLayout = m_inputLayout;
		dto.PixelShader = m_pixelShader;
		dto.ConstantBuffer = m_constantBuffer;
		return dto;
	}

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
};

