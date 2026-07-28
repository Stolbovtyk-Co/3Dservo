#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>

#include "EngineConstants.h"
#include "Node3D.h"
#include "ConvexDecomposer.h"





class Logger;
class FileManager;
class MeshInstance3D;
class ConvexDecomposer;

class MeshFactory
{
public:
	MeshFactory(Microsoft::WRL::ComPtr<ID3D11Device> Device, FileManager* FileMan, Logger* Log) 
		: m_device(Device), m_fileManager(FileMan), m_logger(Log) 
	{
		m_decomposer = std::make_unique<ConvexDecomposer>();
	}

	~MeshFactory() = default;

	std::shared_ptr<Node3D> CreateStaticInstance(std::string PATH);

	std::shared_ptr<Node3D> CreateDynamicInstance(std::string PATH) {} //TODO: Write Later cpuBufferSystem needed

private:
	std::vector<EConst::SubMesh> GetStaticSubMeshes(std::string PATH);

	EConst::CpuMeshDTO GetDynamicMesh(std::string PATH) {} //TODO: Write Later cpuBufferSystem needed

	std::vector<EConst::SubMesh> CreateSubMeshes(EConst::CpuMeshDTO cpu, std::string PATH);
	
	EConst::SubMesh CreateSubMesh(EConst::CpuMeshDTO CpuDTO, EConst::GpuMeshDTO GpuDTO, bool isStatic);

	Logger* m_logger = nullptr;
	FileManager* m_fileManager = nullptr;
	std::unique_ptr<ConvexDecomposer> m_decomposer;
	Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
	std::unordered_map<std::string, std::vector<EConst::GpuMeshDTO>> m_gpuCache; //TODO: Fix
};

