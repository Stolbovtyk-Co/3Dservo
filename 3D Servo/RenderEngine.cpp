#include "RenderEngine.h"
#include <wrl/client.h>
#include <dxgi1_3.h>


void RenderEngine::Initialize(HWND hWnd, int width, int height)
{
	D3D_FEATURE_LEVEL levels[] = {
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
	};

	D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	Microsoft::WRL::ComPtr<ID3D11Device>        device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,                    // Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
		0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		deviceFlags,                // Set debug and Direct2D compatibility flags.
		levels,                     // List of feature levels this app can support.
		ARRAYSIZE(levels),          // Size of the list above.
		D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,                    // Returns the Direct3D device created.
		&m_featureLevel,            // Returns feature level of device created.
		&context                    // Returns the device immediate context.
	);
	device.As(&m_device);
	context.As(&m_context);

	DXGI_SWAP_CHAIN_DESC scd = {};
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 2;
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;   
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    
	scd.OutputWindow = hWnd;                                
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;

	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	m_device.As(&dxgiDevice);

	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;

	hr = dxgiDevice->GetAdapter(&adapter);

	if (SUCCEEDED(hr))
	{
		adapter->GetParent(IID_PPV_ARGS(&factory));

		hr = factory->CreateSwapChain(
			m_device.Get(),
			&scd,
			&m_swapChain
		);
	}

	hr = m_swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&m_pBackBuffer);

	hr = m_device->CreateRenderTargetView(
		m_pBackBuffer.Get(),
		nullptr,
		m_pRenderTarget.GetAddressOf()
	);

	m_pBackBuffer->GetDesc(&m_bbDesc);

	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT> (m_bbDesc.Width),
		static_cast<UINT> (m_bbDesc.Height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	m_device->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&m_pDepthStencil
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	m_device->CreateDepthStencilView(
		m_pDepthStencil.Get(),
		&depthStencilViewDesc,
		&m_pDepthStencilView
	);

	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
	m_viewport.Height = (float)m_bbDesc.Height;
	m_viewport.Width = (float)m_bbDesc.Width;
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;

	m_context->RSSetViewports(
		1,
		&m_viewport
	);
	//Other shit from private
}
