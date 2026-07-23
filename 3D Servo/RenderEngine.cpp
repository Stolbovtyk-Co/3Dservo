#include "RenderEngine.h"
#include <wrl/client.h>
#include <dxgi1_3.h>
#include <iostream>
#include <dwmapi.h>
#include <iostream>
#include <iomanip>

void RenderEngine::Initialize(HWND hWnd, int width, int height)
{
	DWM_BLURBEHIND bb = { 0 };

	// Enable Blur Behind and apply to the entire client area
	bb.dwFlags = DWM_BB_ENABLE;
	bb.fEnable = true;
	bb.hRgnBlur = NULL;

	// Apply Blur Behind
	HRESULT hr;
	//hr = DwmEnableBlurBehindWindow(hWnd, &bb);

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

	hr = D3D11CreateDevice(
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

	DXGI_SWAP_CHAIN_DESC1 scd = {};
	//ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.Width = width;                             // Moved out of BufferDesc
	scd.Height = height;                            // Moved out of BufferDesc
	scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       // Moved out of BufferDesc
	scd.Stereo = FALSE;                          // New: Must be explicitly set
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 2;
	scd.Scaling = DXGI_SCALING_STRETCH;            // New: Specify resizing behavior
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;  // New: Best practice flip model
	scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;        // New: Background transparency
	scd.Flags = 0;

	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	m_device.As(&dxgiDevice);

	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory2> factory;

	hr = dxgiDevice->GetAdapter(&adapter);

	if (SUCCEEDED(hr))
	{
		adapter->GetParent(IID_PPV_ARGS(&factory));

		hr = factory->CreateSwapChainForComposition(
			m_device.Get(),
			&scd,
			nullptr,
			&m_swapChain
		);
		std::cout << "HRESULT: 0x"
			<< std::hex          // Switch to hexadecimal output
			<< std::uppercase    // Use uppercase letters (A-F)
			<< std::setfill('0') // Pad with zeros if the number is short
			<< std::setw(8)      // Ensure the output is exactly 8 characters wide
			<< hr
			<< std::dec          // Reset stream to decimal (good practice)
			<< std::endl;
	}

	DCompositionCreateDevice(dxgiDevice.Get(), IID_PPV_ARGS(&dcompDevice));
	dcompDevice->CreateTargetForHwnd(hWnd, TRUE, &dcompTarget);
	dcompDevice->CreateVisual(&visual);
	visual->SetContent(m_swapChain.Get());
	dcompTarget->SetRoot(visual.Get());
	dcompDevice->Commit();

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

	m_context->OMSetRenderTargets(1, m_pRenderTarget.GetAddressOf(), m_pDepthStencilView.Get()); 
}

void RenderEngine::Clear(float r, float g, float b, float a) {
	float clearColor[] = { r, g, b, a };

	m_context->ClearRenderTargetView(m_pRenderTarget.Get(), clearColor);

	m_context->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderEngine::Render() {
	m_swapChain->Present(1, 0);
}