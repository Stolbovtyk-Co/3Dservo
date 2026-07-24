#include "FileManager.h"

size_t FileManager::ReadBytes(std::string PATH)
{
	HRESULT hr = S_OK;
	FILE* File;

	BYTE* bytes;

	size_t destSize = 0;
	size_t bytesRead = 0;

	errno_t err;
	
	err = fopen_s(&File, "VertexShader.cso", "rb");
	if (err != 0 || &File == nullptr) {
		MessageBoxA(nullptr, "Load Error!", "Pososal!", MB_OK);
		throw;
	}

	fseek(File, 0, SEEK_END);
	destSize = ftell(File);
	fseek(File, 0, SEEK_SET);
	bytes = new BYTE[destSize];

	bytesRead = fread_s(bytes, destSize, 1, destSize, File);

	fclose(File);

	return bytesRead;
}
