#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Scene
{
public:
	
	Scene();

	 virtual void Update(float delta);

	 struct SceneSettings
	 {
		 DirectX::XMFLOAT4 clearColor;
	 };

protected:

	virtual void Setup();
	// RESOURCE MANAGER

	//SETTINGS
	// \ - Clear Color
	// Node tree

	virtual class ShaderManager 
	{
	public:
		ShaderManager();
		virtual struct _constantBufferStruct {};
		struct ShaderDTO
		{
			Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
			Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
		};
		ShaderDTO GetShaderManagerDTO();
	protected:
		virtual void Setup();
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	};

	ShaderManager m_sh;
	SceneSettings m_st;
};

