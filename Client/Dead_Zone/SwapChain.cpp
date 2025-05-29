#include "pch.h"
#include "SwapChain.h"
#include "EnginePch.h"
#include "Engine.h"


void SwapChain::Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	CreateSwapChain(info, dxgi, cmdQueue);
}

void SwapChain::Present()
{
	// Present the frame.
	_swapChain->Present(0, 0);
}

void SwapChain::SwapIndex()
{
	_backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

void SwapChain::CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	ComPtr<IDXGISwapChain> tmp{};

	// 이전에 만든 정보 날린다
	_swapChain.Reset();

    _originalWidth = info.width;
    _originalHeight = info.height;

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = static_cast<uint32>(info.width); // 버퍼의 해상도 너비
	sd.BufferDesc.Height = static_cast<uint32>(info.height); // 버퍼의 해상도 높이
	sd.BufferDesc.RefreshRate.Numerator = 60; // 화면 갱신 비율
	sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 갱신 비율
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링할 것 
	sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 전면+후면 버퍼
	sd.OutputWindow = info.hwnd;
	sd.Windowed = info.windowed;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &tmp);

	tmp.As(&_swapChain);
}

void SwapChain::ChangeSwapChainState(WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	BOOL bFullScreenState;
	_swapChain->GetFullscreenState(&bFullScreenState, nullptr);
	_swapChain->SetFullscreenState(!bFullScreenState, nullptr);

	DXGI_MODE_DESC mdesc{};
	mdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mdesc.Width = info.width;
	mdesc.Height = info.height;
	mdesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	mdesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	_swapChain->ResizeTarget(&mdesc);

	_swapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, info.width, info.height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
}

