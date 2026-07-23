#pragma once
#include <Windows.h>
#include <wrl/client.h>

class ShaderLoader
{
public:
	HRESULT LoadShaders(Microsoft::WRL::ComPtr<ID3D11Device> com_device);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
};

