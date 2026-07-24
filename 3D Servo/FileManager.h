#pragma once
#include <string>
#include <Windows.h>

class FileManager
{
public:
	FileManager() = default;

	struct ReadedBytesDTO {
		BYTE* Bytes;
		size_t BytesRead;
	};

	ReadedBytesDTO ReadBytes(std::string PATH);
private:
};

