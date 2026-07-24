#include "ShaderLoader.h"
#include <d3d11.h>
#include <iostream>
#include <Windows.h>

void ShaderLoader::LoadShaders(Microsoft::WRL::ComPtr<ID3D11Device> com_device)
{
	ID3D11Device* device = com_device.Get();
	HRESULT hr = S_OK;;

	FILE* vShader, * pShader;
	BYTE* bytes;

	size_t destSize;
	size_t bytesRead;

	errno_t err;

	err = fopen_s(&vShader, "VertexShader.cso", "rb");
	std::cout << err << std::endl;
	if (vShader == nullptr) {
		MessageBoxA(nullptr, "NO VertexShader.cso!", "Fatal Error", MB_OK);
		return;
	}

	fseek(vShader, 0, SEEK_END);
	destSize = ftell(vShader);
	fseek(vShader, 0, SEEK_SET);
	bytes = new BYTE[destSize];
	bytesRead = 0;

	bytesRead = fread_s(bytes, destSize, 1, destSize, vShader);
	hr = device->CreateVertexShader(
		bytes,
		bytesRead,
		nullptr,
		&m_vertexShader
	);

	fclose(vShader);

	if (FAILED(hr)) {
		std::cout << bytes;
	}

	D3D11_INPUT_ELEMENT_DESC iaDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		iaDesc,
		ARRAYSIZE(iaDesc),
		bytes,
		bytesRead,
		&m_inputLayout
	);

	if (FAILED(hr)) {
	//	HRErrorHandler::Throw(hr);
	}

	delete bytes;

	err = fopen_s(&pShader, "PixelShader.cso", "rb");
	std::cout << err << std::endl;
	if (pShader == nullptr) {
		MessageBoxA(nullptr, "NO PixelShader.cso!", "Fatal Error", MB_OK);
		return;
	}
	fseek(pShader, 0, SEEK_END);
	destSize = ftell(pShader);
	fseek(pShader, 0, SEEK_SET);
	bytes = new BYTE[destSize];
	bytesRead = 0;

	bytesRead = fread_s(bytes, destSize, 1, destSize, pShader);
	hr = device->CreatePixelShader(
		bytes,
		bytesRead,
		nullptr,
		m_pixelShader.GetAddressOf()
	);
	fclose(pShader);
	if (FAILED(hr)) {
	//	HRErrorHandler::Throw(hr);
	}

	delete bytes;


	CD3D11_BUFFER_DESC cbDesc(
		sizeof(ConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	);

	hr = device->CreateBuffer(
		&cbDesc,
		nullptr,
		m_constantBuffer.GetAddressOf()
	);

	if (!SUCCEEDED(hr)) {
	//	HRErrorHandler::Throw(hr);
	}

	fclose(vShader);
	fclose(pShader);

}
