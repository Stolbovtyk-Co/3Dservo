#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "FileManager.h"
#include <memory>
#include <string>

class Scene
{
public:
	
	Scene(Microsoft::WRL::ComPtr<ID3D11Device> com_device);

	 virtual void Update(float delta);

	 struct SceneSettings
	 {
		 DirectX::XMFLOAT4 clearColor;
		 std::string sceneName;
	 };

	 virtual ~Scene() = default;

	 virtual class ShaderManager
	 {
	 public:
		 ShaderManager(FileManager* flMgr);
		 struct ShaderDTO
		 {
			 Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
			 Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
			 Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
			 Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
		 };
		 ShaderDTO GetShaderManagerDTO();
		 virtual void Setup(Microsoft::WRL::ComPtr<ID3D11Device> com_device);
	 protected:
		 FileManager* m_flMgr;
		 Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		 Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		 Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
		 Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	 };

protected:

	virtual void Setup();

	FileManager m_fileMgr;

	std::unique_ptr <ShaderManager> m_sh;
	SceneSettings m_st;

	//---------------------------------------
	//	IMPORTED FROM RENDER ENGINE CLASSES:
	Microsoft::WRL::ComPtr<ID3D11Device> m_com_device;

	//---------------------------------------
	//	GETTER && SETTER ZONE
public:
	virtual ShaderManager* GetShaderManager() {
		return m_sh.get();
	}
	SceneSettings GetSceneSettings() {
		return m_st;
	}
};

