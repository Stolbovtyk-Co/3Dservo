#pragma once
#include <string>
#include <Windows.h>

class FileManager
{
public:
	FileManager() = default;

	size_t ReadBytes(std::string PATH);
private:
};

