#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include "Logger.h"

class FileManager
{
public:
	FileManager() = default;

	struct ReadedBytesDTO {
		BYTE* Bytes;
		size_t BytesRead;
	};

	ReadedBytesDTO ReadBytes(std::string PATH);

	std::vector<std::string> ReadText(std::string PATH);

private:
	Logger m_logger;
};

