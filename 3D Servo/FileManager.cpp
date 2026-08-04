#include "FileManager.h"
#include <fstream>
#include <filesystem>

FileManager::ReadedBytesDTO FileManager::ReadBytes(std::string PATH)
{
	LOG_INFO(m_logger, "Loading binary file (" + PATH + ").");

	HRESULT hr = S_OK;
	FILE* File;

	ReadedBytesDTO Result;
	size_t destSize = 0;

	errno_t err;
	
	err = fopen_s(&File, PATH.c_str(), "rb");
	if (err != 0 || File == nullptr) {
		LOG_ERROR(m_logger, "Cannot load binary file (" + PATH + ")." );
	}

	fseek(File, 0, SEEK_END);
	destSize = ftell(File);
	fseek(File, 0, SEEK_SET);
	Result.Bytes = new BYTE[destSize];

	Result.BytesRead = fread_s(Result.Bytes, destSize, 1, destSize, File);

	fclose(File);

	return Result;
}

std::vector<std::string> FileManager::ReadText(std::string PATH)
{
	LOG_INFO(m_logger, "Loading text file (" + PATH + ").");
	
	if (!std::filesystem::exists(PATH)) {
		LOG_ERROR(m_logger,"File not found at PATH: " + PATH);
	}

	std::ifstream file(PATH);
	if (!file.is_open()) {
		LOG_ERROR(m_logger, "Cannot load text file (" + PATH + ").");
	}
	std::vector<std::string> lines;
	std::string line;

	while (std::getline(file, line)) {
		lines.push_back(line);
	}

	file.close();
	
	return lines;
}
