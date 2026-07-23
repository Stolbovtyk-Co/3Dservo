#include "ShaderLoader.h"
#include <d3d11.h>
#include <iostream>
#include <fstream>
#include "HRErrorHandler.h"

void ShaderLoader::LoadShaders(Microsoft::WRL::ComPtr<ID3D11Device> com_device)
{
	ID3D11Device* device = com_device.Get();
	HRESULT hr = S_OK;;

	FILE* vShader, * pShader;
	BYTE* bytes;

	size_t destSize = 4096;
	size_t bytesRead = 0;
	bytes = new BYTE[destSize];

	fopen_s(&vShader, "VertexShader.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, 4096, vShader);
	hr = device->CreateVertexShader(
		bytes,
		bytesRead,
		nullptr,
		&m_vertexShader
	);
	if (!SUCCEEDED(hr)) {
	//	HRErrorHandler::Throw(hr);
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

	if (!SUCCEEDED(hr)) {
	//	HRErrorHandler::Throw(hr);
	}

	delete bytes;

	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&pShader, "PixelShader.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, 4096, pShader);
	hr = device->CreatePixelShader(
		bytes,
		bytesRead,
		nullptr,
		m_pixelShader.GetAddressOf()
	);

	if (!SUCCEEDED(hr)) {
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
