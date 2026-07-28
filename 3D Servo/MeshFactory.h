#pragma once
#include <unordered_map>
#include <memory>
#include <string>

#include "EngineConstants.h"
#include "Node3D.h"

class Logger;
class FileManager;
class MeshInstance3D;

class MeshFactory
{
public:
	MeshFactory(Microsoft::WRL::ComPtr<ID3D11Device> Device, FileManager* FileMan, Logger* Log) : m_device(Device), m_fileManager(FileMan), m_logger(Log) {}

	~MeshFactory() = default;

	std::shared_ptr<Node3D> CreateStaticInstance(std::string PATH);

	std::shared_ptr<Node3D> CreateDynamicInstance(std::string PATH) {} //TODO: Write Later cpuBufferSystem needed

private:
	EConst::GpuMeshDTO GetStaticMesh(std::string PATH);

	EConst::CpuMeshDTO GetDynamicMesh(std::string PATH) {} //TODO: Write Later cpuBufferSystem needed

	EConst::GpuMeshDTO CreateD3DBuffers(EConst::CpuMeshDTO dto);

	Logger* m_logger = nullptr;
	FileManager* m_fileManager = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
	std::unordered_map<std::string, EConst::GpuMeshDTO> m_gpuCache;
};

