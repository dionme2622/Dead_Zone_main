#pragma once

class SwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Present();
	void SwapIndex();

	ComPtr<IDXGISwapChain3> GetSwapChain() { return _swapChain; }
	uint8 GetBackBufferIndex() { return _swapChain->GetCurrentBackBufferIndex(); }
	//uint8 GetBackBufferIndex() { return _backBufferIndex; }

	void ChangeSwapChainState(WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);
private:
	void CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);


private:
	ComPtr<IDXGISwapChain3>	_swapChain;

	uint32					_backBufferIndex = 0;

	int32					_originalWidth;
	int32 					_originalHeight;
};

