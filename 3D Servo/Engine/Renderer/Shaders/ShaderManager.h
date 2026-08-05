#pragma once
#include "Renderer/Device/GraphicsDevice.h"
#include "Importers/FileManager.h"
#include "Core/Logger.h"
#include <unordered_map>

class ShaderManager
{
public:
	void Init(GraphicsDevice* gDevice, FileManager* flMgr);
	void CreateVertexShader(std::string PATH, std::string Name);
	void CreatePixelShader(std::string PATH, std::string Name);
	void CreateSampler(std::string Name);

	void SetVS(std::string name, bool fs = true);
	void SetPS(std::string name, bool fs = true);
	void SetIL(std::string name, bool fs = true);
	void SetSampler(std::string name, bool fs = true);
private:
	void CreateInputLayout(std::string Name, FileManager::ReadedBytesDTO vShader);

	GraphicsDevice* m_gDevice;
	FileManager* m_flMgr;
	Logger m_logger;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> m_VS_DB;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> m_PS_DB;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> m_IL_DB;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_Sampler_DB;
};