#pragma once

enum
{
	SAMPLER_DESC_COUNT = 2
};

class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return _graphicsRootSignature; }
	ComPtr<ID3D12RootSignature>	GetComputeRootSignature() { return _computeRootSignature; }

private:
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

private:
	array<D3D12_STATIC_SAMPLER_DESC, SAMPLER_DESC_COUNT>	_samplerDescs;
	D3D12_STATIC_SAMPLER_DESC								_samplerDesc;
	D3D12_STATIC_SAMPLER_DESC								_shadowSamplerDesc;
	ComPtr<ID3D12RootSignature>								_graphicsRootSignature;
	ComPtr<ID3D12RootSignature>								_computeRootSignature;
};

