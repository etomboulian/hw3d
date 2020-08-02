#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define GFX_EXECPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, hr)
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)

// if we are in debug mode then we will include the detailed Error Messages in an infoManager object
#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED(hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__,__FILE__, v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

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
	sd.Windowed = TRUE;	 										// is Windowed
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr;

	GFX_THROW_INFO(
		D3D11CreateDeviceAndSwapChain(	
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr,
		swapCreateFlags, 
		nullptr,
		0, 
		D3D11_SDK_VERSION, 
		&sd,
		&pSwap, 
		&pDevice, 
		nullptr, 
		&pContext 
		) 
	);

	// gain access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));
}


void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED) 
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

void Graphics::DrawTestTriangle(float angle, float x, float y)
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
				
	};

	const Vertex verticies[] =
	{
		{-1.0f,		-1.0f,	-1.0f},		
		{1.0f,		-1.0f,	-1.0f},		
		{-1.0f,		 1.0f,	-1.0f},
		{1.0f,		 1.0f,	-1.0f},
		{-1.0f,		-1.0f,	 1.0f},
		{1.0f,		-1.0f, 	 1.0f},
		{-1.0f,		 1.0f,	 1.0f},
		{1.0f,		 1.0f,	 1.0f},
	};

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(verticies);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = verticies;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));



	// Bind vertex buffer to the pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;

	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	// Data for Index buffer;
	const unsigned short indicies[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	// Create an index buffer and provide the required configuration
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indicies);
	ibd.StructureByteStride = sizeof(unsigned short);

	// Create an index buffer subresource data struct
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indicies;

	// Create the actual index buffer
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// bind the created index buffer
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// Data for constant buffer
	struct ConstantBuffer
	{
		dx::XMMATRIX transform;
	};

	const ConstantBuffer cb =
	{
		dx::XMMatrixTranspose
		(
			dx::XMMatrixRotationZ(angle) * 
			dx::XMMatrixRotationX(angle) *
			dx::XMMatrixTranslation( x, y, 4.0f) *
			dx::XMMatrixPerspectiveLH( 1.0f, 3.0/4.0f, 0.5f, 10.f)
		)
		
	};

	// Configure some data for the constant buffer
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;

	// Create the constant buffer
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));	

	// bind constant buffer to vertex shader
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// Create a second constant buffer about colors
	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};
	// Fill the constant buffer with data
	const ConstantBuffer2 cb2 =
	{
		{
			{1.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 1.0f},
		}
	};

	// Create the constant buffer
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer2;
	D3D11_BUFFER_DESC cbd2;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.CPUAccessFlags = 0u;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));

	// Bind the second constant buffer to the pixel shader
	pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());

	// Create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"src\\PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// Bind Pixel Shader
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// Create a Vertex Shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"src\\VertexShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// Bind Vertex Shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// input (vertex) layout (2d position only)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{
			"Position",						// semantic name 
			0,								// index of semantic
			DXGI_FORMAT_R32G32B32_FLOAT,		// Format
			0,								// Input Slot
			0,								// aligned Byte Offset
			D3D11_INPUT_PER_VERTEX_DATA,	// Input slot class
			0								// InstanceDataStepRate
		},
		
	};

	// Create the vertex input Layout
	GFX_THROW_INFO(
		pDevice->CreateInputLayout(
			ied,
			(UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout
		)
	);

	// Bind vertex layout
	pContext->IASetInputLayout(pInputLayout.Get());

	// Bind render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(pContext->DrawIndexed( (UINT)std::size(indicies), 0u, 0u));
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file), hr(hr)
{
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}

	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

inline Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file)
{
	// join all of the messages with new lines in to a single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	if (!info.empty())
	{
		info.pop_back();
	}
}
