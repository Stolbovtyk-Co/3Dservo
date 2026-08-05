#pragma once
#include "Core/Logger.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <dxgi1_2.h> 
#include <dcomp.h> 

class GraphicsDevice
{
public:
	void Init(HWND hWnd, int Width, int Height);

	ID3D11Device* GetDevice() { return m_device.Get(); }
	ID3D11DeviceContext* GetContext() { return m_context.Get(); }

	void FrameRenderBegin(DirectX::XMFLOAT4 cc);
	void FrameRenderRegular();
	void FrameRenderTransparent();
	void FrameRenderFinish();
private:
	void CreatePipelineStates(bool& retFlag);
	void CreateRenderTargetsAndDepthStencil(bool& retFlag);
	void CreateDirectComposition(HWND hWnd, bool& retFlag);
	void CreateD3D11SwapChain(int Width, int Height, bool& retFlag);
	void CreateD3D11Device(bool& retFlag);

	Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context = nullptr;
	Microsoft::WRL::ComPtr <IDXGISwapChain1> m_swapChain = nullptr;

	Microsoft::WRL::ComPtr<IDCompositionDevice> dcompDevice;
	Microsoft::WRL::ComPtr<IDCompositionTarget> dcompTarget;
	Microsoft::WRL::ComPtr<IDCompositionVisual> visual;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBackBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget = nullptr;
	D3D11_TEXTURE2D_DESC m_bbDesc;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_pDepthStencil = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;
	D3D11_VIEWPORT m_viewport;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ID3D11DepthStencilState* pDepthStencilStateRegular = nullptr;
	ID3D11DepthStencilState* pDepthStencilStateTransparent = nullptr;
	ID3D11BlendState* m_pBlendStateTransparent = nullptr;

	Logger m_logger;

	D3D_FEATURE_LEVEL m_levels[7] = {
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
	};
};

