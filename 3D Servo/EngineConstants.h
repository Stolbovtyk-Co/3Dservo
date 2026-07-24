#pragma once
class EngineConstants
{
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>
public:
	typedef struct _constantBufferStruct {
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	} ConstantBufferStruct;
	long GetSizeOfConstantBufferStruct() { return m_sizeOfConstantBufferStruct; }

private:
	long m_sizeOfConstantBufferStruct = sizeof(ConstantBufferStruct);
};

