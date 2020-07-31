#include "Graphics.h"

#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	// create swapchain descriptor
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;									// let the window decide
	sd.BufferDesc.Height = 0;									// let the window decide
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;			// format of the pixels
	sd.BufferDesc.RefreshRate.Numerator = 0;					// Use the existing refresh rate
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;		// No Scaling
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;									// Anti-aliasing (disabled)
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			// Front Buffer
	sd.BufferCount = 1;											// number of back buffers
	sd.OutputWindow = hWnd;										// window to output to
	sd.Windowed = TRUE;											// is Windowed
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr,
		0, 
		nullptr,
		0, 
		D3D11_SDK_VERSION, 
		&sd,
		&pSwap, 
		&pDevice, 
		nullptr, 
		&pContext
	);

	// gain access to texture subresource in swap chain (back buffer)
	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&pTarget
	);
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pTarget != nullptr)
	{
		pTarget->Release();
	}
	if (pContext != nullptr)	// if there is a pContext, release it
	{
		pContext->Release();
	}
	if (pSwap != nullptr)		// if there is a pSwap, release it
	{
		pSwap->Release();
	}
	if (pDevice != nullptr)		// if there is a pDevice, release it
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}
