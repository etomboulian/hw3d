#pragma once
#include "ChiliWin.h"
#include <d3d11.h>


class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;			// remove the default copy constructor
	Graphics& operator=(const Graphics&) = delete;	// remove the default overloaded assignment op
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		const float color[] = { red, green, blue, 1.0f };
		pContext->ClearRenderTargetView(pTarget, color);
	}
private: 
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};

