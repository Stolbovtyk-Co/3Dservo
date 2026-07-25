#include "FileManager.h"

FileManager::ReadedBytesDTO FileManager::ReadBytes(std::string PATH)
{
	HRESULT hr = S_OK;
	FILE* File;

	ReadedBytesDTO Result;
	size_t destSize = 0;

	errno_t err;
	
	err = fopen_s(&File, PATH.c_str(), "rb");
	if (err != 0 || File == nullptr) {
		MessageBoxA(nullptr, "Load Error!", "Pososal!", MB_OK);
		throw;
	}

	fseek(File, 0, SEEK_END);
	destSize = ftell(File);
	fseek(File, 0, SEEK_SET);
	Result.Bytes = new BYTE[destSize];

	Result.BytesRead = fread_s(Result.Bytes, destSize, 1, destSize, File);

	fclose(File);

	return Result;
}
