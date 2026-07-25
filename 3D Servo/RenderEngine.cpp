#include "RenderEngine.h"
#include <DirectXMath.h>
#include <dxgi1_3.h>
#include <wrl/client.h>

using namespace DirectX;

#pragma comment(lib, "dcomp.lib")


RenderEngine::RenderEngine(std::shared_ptr<Logger> logger, HWND hWnd, int Width, int Height)
{
	m_logger = logger;
	Initialize(hWnd, Width, Height);
	LoadScene(std::make_unique<Scene>(m_device));
	LoadSceneSettings();
	CreateViewAndPerspective(); // TODO: move this to scene
	m_frameCount = 0;
}

void RenderEngine::Initialize(HWND hWnd, int Width, int Height)
{
	HRESULT hr = S_OK;

	m_width = Width;
	m_height = Height;

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
	if (FAILED(hr)) {
		m_logger->logHR(hr);
		return;
	}

	device.As(&m_device);
	context.As(&m_context);

	DXGI_SWAP_CHAIN_DESC1 scd = {};
	scd.Width = m_width;                             // Moved out of BufferDesc
	scd.Height = m_height;                            // Moved out of BufferDesc
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
	if (FAILED(hr)) {
		m_logger->logHR(hr);
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
		m_logger->logHR(hr);
		return;
	}

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthClipEnable = TRUE;

	hr = m_device->CreateRasterizerState(&rasterDesc, m_rasterizerState.GetAddressOf());
	if (FAILED(hr)) {
		m_logger->logHR(hr);
		return;
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
	if (FAILED(hr)) {
		m_logger->logHR(hr);
		return;
	}

	hr = m_device->CreateRenderTargetView(
		m_pBackBuffer.Get(),
		nullptr,
		m_pRenderTarget.GetAddressOf()
	);
	if (FAILED(hr)) {
		m_logger->logHR(hr);
		return;
	}

	m_pBackBuffer->GetDesc(&m_bbDesc);

	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT> (m_bbDesc.Width),
		static_cast<UINT> (m_bbDesc.Height),
		1,
		1,
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
}

void RenderEngine::Clear(float r, float g, float b, float a) {
	float clearColor[] = { r * a, g * a, b * a, a };

	m_context->ClearRenderTargetView(m_pRenderTarget.Get(), clearColor);

	m_context->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderEngine::LoadSceneSettings()
{
	Scene::SceneSettings settings = m_currentScene.get()->GetSceneSettings();
	m_logger.get()->log("Loaded scene: " + settings.sceneName);
	m_clearColor = settings.clearColor;
}

void RenderEngine::Update()
{
	m_frameCount++;

	XMMATRIX finalRotation = DirectX::XMMatrixRotationY(
		DirectX::XMConvertToRadians(
			(float)m_frameCount
		)
	) * DirectX::XMMatrixRotationX(
		DirectX::XMConvertToRadians(
			(float)m_frameCount
		)
	) * DirectX::XMMatrixRotationZ(
		DirectX::XMConvertToRadians(
			(float)m_frameCount / 2
		)
	);
	
	DirectX::XMStoreFloat4x4(
		&m_constantBufferData.world,
		finalRotation
	);

	if (m_frameCount == MAXUINT)  m_frameCount = 0;
}

void RenderEngine::Render() {

	RenderEngine::Clear(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
	// Update  `m_contantBufferData`, `m_pConstantBuffer.Get()`
	// Clear  Application::Risovat() //skip
	// Settings `VertexPositionColor`, `m_pVertexBuffer` ,`m_pIndexBuffer`, 
	// VertexShader ShaderLoader::Load->GetVertex(), `m_pConstantBuffer`
	// PixelShader ShaderLoader::Load->GetPixel()

	m_context->UpdateSubresource(
		m_constantBuffer.Get(),
		0,
		nullptr,
		&m_constantBufferData,
		0,
		0
	);

	m_context->OMSetRenderTargets(1, m_pRenderTarget.GetAddressOf(), m_pDepthStencilView.Get());
	

	UINT stride = sizeof(BufferLoader::VertexPositionColor);
	UINT offset = 0;

	m_context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);
	
		m_context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0
	);

	m_context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);
	
		m_context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);
	
	m_context->IASetInputLayout(m_inputLayout.Get());

	m_context->VSSetConstantBuffers(
		0,
		1,
		m_constantBuffer.GetAddressOf()
	);
	
	m_context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	m_context->RSSetState(m_rasterizerState.Get());

	m_context->DrawIndexed(
		m_bufferLoader.GetIndexCount(),
		0,
		0
	);

	m_swapChain->Present(1, 0);
}

void RenderEngine::CreateViewAndPerspective()
{
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, 1.5f, 0.f); 
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

	DirectX::XMStoreFloat4x4(
		&m_constantBufferData.view,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixLookAtRH( 
				eye,
				at,
				up
			)
		)
	);

	float aspectRatioX = m_width / m_height;
	float aspectRatioY = aspectRatioX < (16.0f / 9.0f) ? aspectRatioX / (16.0f / 9.0f) : 1.0f;

	DirectX::XMStoreFloat4x4(
		&m_constantBufferData.projection,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixPerspectiveFovRH(
				DirectX::XMConvertToRadians(70), 
				aspectRatioX,
				0.01f,
				100.0f
			)
		)
	);
}

/// <summary>
/// Not Async ¯\_(ツ)_/¯, But Loading Shaders & Buffers
/// </summary>
void RenderEngine::PreloadAssetsAsync() //TODO: Asynch Loading
{
#pragma region Shaders
	m_currentScene.get()->GetShaderManager()->Setup(m_device);
 	Scene::ShaderManager::ShaderDTO ShaderDTO = m_currentScene.get()->GetShaderManager()->GetShaderManagerDTO();
	m_vertexShader = ShaderDTO.vertexShader;
	m_inputLayout = ShaderDTO.inputLayout;
	m_pixelShader = ShaderDTO.pixelShader;
	m_constantBuffer = ShaderDTO.constantBuffer;
#pragma endregion

#pragma region Buffers
	m_bufferLoader.LoadBuffer(m_device);
	BufferLoader::BuffersStaffDTO BuffDTO = m_bufferLoader.GetBuffersStaff();
	m_vertexBuffer = BuffDTO.VertexBuffer;
	m_indexBuffer = BuffDTO.IndexBuffer;
#pragma endregion
}
