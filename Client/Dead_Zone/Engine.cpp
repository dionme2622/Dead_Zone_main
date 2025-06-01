#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "KeyInput.h"
#include "MouseInput.h"
#include "Camera.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Light.h"
#include "Resources.h"
#include "InstancingManager.h"
#include "PhysicsSystem.h"
#include "DebugRenderer.h"

void Engine::Init(const WindowInfo& info)
{
	_window = info;

	// 그려질 화면 크기를 설정
	_viewport = { 0, 0, static_cast<FLOAT>(_window.width), static_cast<FLOAT>(_window.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, _window.width, _window.height);

	_device->Init();
	_graphicsCmdQueue->Init(_device->GetDevice(), _swapChain);
	_computeCmdQueue->Init(_device->GetDevice());
	_swapChain->Init(_window, _device->GetDevice(), _device->GetDXGI(), _graphicsCmdQueue->GetCmdQueue());
	_rootSignature->Init();
	_graphicsDescHeap->Init(256 * 30);
	_computeDescHeap->Init();

	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256 * 100);
	CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256 * 100);

	CreateRenderTargetGroups();
	ResizeWindow(_window.width, _window.height);
	//ToggleFullscreen();

	GET_SINGLE(KeyInput)->Init(_window.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->Init();
	GET_SINGLE(PhysicsSystem)->Init();
	GET_SINGLE(DebugRenderer)->Init(DEVICE, 10000);
}

void Engine::Update()
{
	if (INPUT->GetButtonDown(KEY_TYPE::F9)) {
		ToggleFullscreen();
	}

	GET_SINGLE(KeyInput)->Update();
	GET_SINGLE(Timer)->Update();
	GET_SINGLE(SceneManager)->Update();
	GET_SINGLE(InstancingManager)->ClearBuffer();

	Render();

	ShowFps();
}

void Engine::Render()
{
	RenderBegin();

	GET_SINGLE(SceneManager)->Render();

	RenderEnd();
}

void Engine::RenderBegin()
{
	_graphicsCmdQueue->RenderBegin();
}

void Engine::RenderEnd()
{
	_graphicsCmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int32 width, int32 height)
{
	_window.width = width;
	_window.height = height;

	RECT rect = { 0, 0, width, height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	::SetWindowPos(_window.hwnd, 0, 0, 0, width, height, 0);
}

void Engine::ShowFps()
{
	uint32 fps = GET_SINGLE(Timer)->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(_window.hwnd, text);
}

void Engine::CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count)
{
	uint8 typeInt = static_cast<uint8>(reg);
	assert(_constantBuffers.size() == typeInt);

	shared_ptr<ConstantBuffer> buffer = make_shared<ConstantBuffer>();
	buffer->Init(reg, bufferSize, count);
	_constantBuffers.push_back(buffer);
}


void Engine::CreateRenderTargetGroups()
{
	static int j = 0;

	// DepthStencil
	_dsTexture = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil",
		DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	
	// SwapChain Group
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + to_wstring(i);
			ComPtr<ID3D12Resource> resource;
			_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
			rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
		}

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, _dsTexture);
	}

	// Shadow Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"ShadowTarget",
			DXGI_FORMAT_R32_FLOAT, 2048, 2048,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		shared_ptr<Texture> shadowDepthTexture = GET_SINGLE(Resources)->CreateTexture(L"ShadowDepthStencil",
			DXGI_FORMAT_D32_FLOAT, 4096 * 2, 4096 * 2,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec, shadowDepthTexture);
	}

	// Deferred Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"PositionTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"NormalTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[2].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, _dsTexture);
	}

	// Lighting Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"SpecularLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[2].target = GET_SINGLE(Resources)->CreateTexture(L"MetallicLightTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING, rtVec, _dsTexture);
	}

	// Post Processing Group
	/*{
		vector<RenderTarget> rtVec(RENDER_TARGET_POST_PROCCESING_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"PostProcessTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::POST_PROCESSING)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::POST_PROCESSING)]->Create(RENDER_TARGET_GROUP_TYPE::POST_PROCESSING, rtVec, dsTexture);
	}*/


	//// Final Render Group
	//{
	//	vector<RenderTarget> rtVec(RENDER_TARGET_BLUR_GROUP_MEMBER_COUNT);

	//	rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"FinalTarget",
	//		DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
	//		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	//		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	//	shared_ptr<Texture> blurDepthTexture = GET_SINGLE(Resources)->CreateTexture(L"FinalDepthStencil",
	//		DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
	//		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	//		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	//	_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::BLUR)] = make_shared<RenderTargetGroup>();
	//	_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::BLUR)]->Create(RENDER_TARGET_GROUP_TYPE::BLUR, rtVec, blurDepthTexture);
	//}
}

void Engine::ToggleFullscreen()
{
    ReleaseRenderTargets();
	
    _swapChain->ChangeSwapChainState(_window,
                                     _device->GetDXGI(),
                                     _graphicsCmdQueue->GetCmdQueue());
	
    CreateRenderTargetGroups();


	_graphicsCmdQueue->FlushResourceCommandQueue();
	_computeCmdQueue->FlushComputeCommandQueue();

	_graphicsCmdQueue->WaitSync();
	_computeCmdQueue->WaitSync();
}

void Engine::ReleaseRenderTargets()
{
	for (auto& grp : _rtGroups)
	{
		if (!grp) continue;
		
		const uint32 rtCount = static_cast<uint32>(grp->GetRTCount());

		for (uint32 i = 0; i < rtCount; ++i)
		{
			auto& tex = grp->GetRTTexture(i);
			if (tex)
				tex->GetTex2D().Reset();
		}
		
		auto& dsTex = grp->GetDSTexture();

		if (dsTex)
			dsTex->GetTex2D().Reset();

		// grp.reset();
	}

	_rtGroups.fill(nullptr);

	 //_dsTexture->GetTex2D().Reset();
}


// Texture Add할 때, 새로 만든 텍스처도 같은 이름을 사용하기 때문에 mapping이 되지 않았다.
// 지금 이 소스 265번 라인 주석 없애면 죽는다. => Reset한 텍스처에다 그렸기 때문 아닐까? 그렇다면, 새로 바뀐 텍스처에다 렌더링을 하지 않는다는 소리다.
// ResizeBuffer한 다음에 Swap chain index가 바뀌었을 수 있다. back buffer index는 IDXGISwapChain3::GetCurrentBackBufferIndex()로 얻자.
// 스마트 포인터들 reset하거나 대입하고 싶을 때, 복사본에다 reset하거나 대입해봐야 의미가 없다. 안 바뀐다.
//		=> 스마트 포인터 getter를 제공할 때에는, 정말로 복사를 의도한 게 아닌 이상 다 레퍼런스로 반환하도록 하자.
//
// 그림자 과련 메모리에 데이터 매핑을 하는데 사이즈랑 맞지 않아서 다른 메모리에 침범해서 텍스처가 깨질수 있다.


// Create할 때 만들었던 사이즈보다,
// PushGraphicsData 등으로 mapping된 포인터에 memcpy할 때 memcpy하는 사이즈가 커서, 잡혀있는 메모리를 초과했을 가능성이 있다.
// 그래서 옆에 메모리를 막 침범했을 수 있다.