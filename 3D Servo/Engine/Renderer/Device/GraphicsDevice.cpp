#include "GraphicsDevice.h"
#include<dcomp.h>


void GraphicsDevice::Init(HWND hWnd, int Width, int Height)
{
	bool retFlag;
	CreateD3D11Device(retFlag);
	if (retFlag) return;
	CreateD3D11SwapChain(Width, Height, retFlag);
	if (retFlag) return;
	CreateDirectComposition(hWnd, retFlag);
	if (retFlag) return;
	CreateRenderTargetsAndDepthStencil(retFlag);
	if (retFlag) return;
	CreatePipelineStates(retFlag);
	if (retFlag) return;
}

void GraphicsDevice::FrameRenderBegin(DirectX::XMFLOAT4 cc)
{
	float clearColor[] = { cc.x * cc.w, cc.y * cc.w, cc.z * cc.w, cc.w };
	m_context->ClearRenderTargetView(m_pRenderTarget.Get(), clearColor);
	m_context->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_context->OMSetRenderTargets(1, m_pRenderTarget.GetAddressOf(), m_pDepthStencilView.Get());
}

void GraphicsDevice::FrameRenderRegular()
{
	m_context->OMSetBlendState(nullptr, NULL, 0xffffffff);
	m_context->OMSetDepthStencilState(pDepthStencilStateRegular, 1);
}

void GraphicsDevice::FrameRenderTransparent()
{
	m_context->OMSetBlendState(m_pBlendStateTransparent, NULL, 0xffffffff);
	m_context->OMSetDepthStencilState(pDepthStencilStateTransparent, 1);
}

void GraphicsDevice::FrameRenderFinish()
{
	m_swapChain->Present(1, 0);
}

void GraphicsDevice::CreatePipelineStates(bool& retFlag)
{
	retFlag = true;
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = TRUE;
	rasterDesc.DepthClipEnable = TRUE;

	HRESULT hr = m_device->CreateRasterizerState(&rasterDesc, m_rasterizerState.GetAddressOf());
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE RASTERIZER STATE!");
		m_logger.logHR(hr);
		return;
	}
	m_context->RSSetState(m_rasterizerState.Get());

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = m_device->CreateBlendState(&blendDesc, &m_pBlendStateTransparent);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE BLEND STATE!");
		m_logger.logHR(hr);
		return;
	}

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = m_device->CreateDepthStencilState(&dsDesc, &pDepthStencilStateRegular);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE DEPTH STENCIL STATE REGULAR!");
		m_logger.logHR(hr);
		return;
	}

	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(&dsDesc, &pDepthStencilStateTransparent);

	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE DEPTH STENCIL STATE TRANSPARENT!");
		m_logger.logHR(hr);
		return;
	}
	retFlag = false;
}

void GraphicsDevice::CreateRenderTargetsAndDepthStencil(bool& retFlag)
{
	retFlag = true;
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pBackBuffer);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO GET SWAP CHAIN BUFFER!");
		m_logger.logHR(hr);
		return;
	}

	hr = m_device->CreateRenderTargetView(m_pBackBuffer.Get(), nullptr, m_pRenderTarget.GetAddressOf());
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE RENDER TAGET VIEW!");
		m_logger.logHR(hr);
		return;
	}

	m_pBackBuffer->GetDesc(&m_bbDesc);

	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>(m_bbDesc.Width),
		static_cast<UINT>(m_bbDesc.Height),
		1, 1, D3D11_BIND_DEPTH_STENCIL
	);

	hr = m_device->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencil);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE TEXTURE2D!");
		m_logger.logHR(hr);
		return;
	}

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	hr = m_device->CreateDepthStencilView(m_pDepthStencil.Get(), &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE DEPTH STENCIL!");
		m_logger.logHR(hr);
		return;
	}

	ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
	m_viewport.Height = static_cast<float>(m_bbDesc.Height);
	m_viewport.Width = static_cast<float>(m_bbDesc.Width);
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;

	m_context->RSSetViewports(1, &m_viewport);
	retFlag = false;
}

void GraphicsDevice::CreateDirectComposition(HWND hWnd, bool& retFlag)
{
	retFlag = true;
	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	m_device.As(&dxgiDevice);

	HRESULT hr = DCompositionCreateDevice(dxgiDevice.Get(), IID_PPV_ARGS(&dcompDevice));
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE COMPOSITON DEVICE!");
		m_logger.logHR(hr);
		return;
	}

	hr = dcompDevice->CreateTargetForHwnd(hWnd, TRUE, &dcompTarget);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE COMPOSITON DEVICE TARGET!");
		m_logger.logHR(hr);
		return;
	}

	hr = dcompDevice->CreateVisual(&visual);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE COMPOSITON DEVICE VISUAL!");
		m_logger.logHR(hr);
		return;
	}

	hr = visual->SetContent(m_swapChain.Get());
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO SET COMPOSITON DEVICE CONSTENT!");
		m_logger.logHR(hr);
		return;
	}

	hr = dcompTarget->SetRoot(visual.Get());
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO SET ROOT OF COMPOSITON D3D11 DEVICE!");
		m_logger.logHR(hr);
		return;
	}

	hr = dcompDevice->Commit();
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO COMMIT COMPOSITON D3D11 DEVICE!");
		m_logger.logHR(hr);
		return;
	}
	retFlag = false;
}

void GraphicsDevice::CreateD3D11SwapChain(int Width, int Height, bool& retFlag)
{
	retFlag = true;
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory2> factory;
	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	m_device.As(&dxgiDevice);

	DXGI_SWAP_CHAIN_DESC1 scd = {};
	scd.Width = static_cast<UINT>(Width);
	scd.Height = static_cast<UINT>(Height);
	scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.Stereo = FALSE;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 2;
	scd.Scaling = DXGI_SCALING_STRETCH;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	scd.Flags = 0;

	hr = dxgiDevice->GetAdapter(&adapter);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE D3D11 DEVICE ADAPTER!");
		m_logger.logHR(hr);
		return;
	}
	adapter->GetParent(IID_PPV_ARGS(&factory));

	hr = factory->CreateSwapChainForComposition(
		m_device.Get(),
		&scd,
		nullptr,
		&m_swapChain
	);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE D3D11 SWAP CHAIN!");
		m_logger.logHR(hr);
		return;
	}
	retFlag = false;
}

void GraphicsDevice::CreateD3D11Device(bool& retFlag)
{
	retFlag = true;
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		deviceFlags,
		m_levels,
		ARRAYSIZE(m_levels),
		D3D11_SDK_VERSION,
		&m_device,
		&m_featureLevel,
		&m_context
	);
	if (FAILED(hr)) {
		LOG_FATAL(m_logger, "FAILED TO CREATE D3D11 DEVICE!");
		m_logger.logHR(hr);
		return;
	}
	retFlag = false;
}
