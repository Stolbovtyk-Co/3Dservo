#include "RenderEngine.h"

void RenderEngine::Initialize(HWND hWnd, int width, int height)
{
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 1;
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;   
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    
	scd.OutputWindow = hWnd;                                
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;

	//Other shit from private
}
