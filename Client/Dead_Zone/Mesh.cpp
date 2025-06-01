#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "BinaryLoader.h"
#include "StructuredBuffer.h"
#include "InstancingBuffer.h"
#include "Resources.h"

Mesh::Mesh() : Object(OBJECT_TYPE::MESH)
{

}

Mesh::~Mesh()
{

}

void Mesh::Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexBuffer);
}


void Mesh::Render(uint32 instanceCount, uint32 idx, bool isRender)
{
	if (isRender)
	{
		GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
		GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

		GEngine->GetGraphicsDescHeap()->CommitTable();

		GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);
	}
}

void Mesh::Render(shared_ptr<InstancingBuffer>& buffer, uint32 idx, bool isRender)
{
	if (isRender)
	{
		D3D12_VERTEX_BUFFER_VIEW bufferViews[] = { _vertexBufferView, buffer->GetBufferView() };
		GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferViews);
		GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

		GEngine->GetGraphicsDescHeap()->CommitTable();

		GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, buffer->GetCount(), 0, 0, 0);
	}
}

shared_ptr<Mesh> Mesh::CreateFromBinary(const BinaryMeshInfo* meshInfo, BinaryLoader& loader, int type)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	if (!meshInfo->vertices.empty())
	{
		mesh->CreateVertexBuffer(meshInfo->vertices);

	}
	for (const vector<uint32>& buffer : meshInfo->indices)
	{
		if (buffer.empty())
		{
			// FBX 파일이 이상하다. IndexBuffer가 없으면 에러 나니까 임시 처리
			vector<uint32> defaultBuffer{ 0 };
			mesh->CreateIndexBuffer(defaultBuffer);
		}
		else
		{
			mesh->CreateIndexBuffer(buffer);
		}
	}

	if (meshInfo->hasAnimation)
	{
		mesh->CreateBonesAndAnimations(loader, type);
		mesh->_hasAnimation = true;
	}

	return mesh;
}


void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer)
{
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));

	// Copy the triangle data to the vertex buffer.
	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	_vertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
	uint32 indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> indexBuffer;
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer));

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	indexBuffer->Unmap(0, nullptr);

	D3D12_INDEX_BUFFER_VIEW	indexBufferView;
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = bufferSize;

	IndexBufferInfo info =
	{
		indexBuffer,
		indexBufferView,
		DXGI_FORMAT_R32_UINT,
		indexCount
	};

	_vecIndexInfo.push_back(info);
}

void Mesh::CreateBonesAndAnimations(class BinaryLoader& loader, int type)	
{
#pragma region AnimClip
	uint32 frameCount = 0;
	vector<shared_ptr<BinaryAnimClipInfo>>& animClips = loader.GetAnimClip();			// Loader의 애니메이션 클립들을 가져온다.
	for (shared_ptr<BinaryAnimClipInfo>& ac : animClips)
	{
		shared_ptr<AnimClipInfo> info = make_shared<AnimClipInfo>();

		info->animName = ac->name;
		info->duration = ac->duration;

		info->frameCount = ac->frameCount;

		info->keyFrames.resize(ac->keyFrames.size());
		const int32 keyFrameCount = static_cast<int32>(ac->keyFrames.size());
		for (int32 f = 0; f < keyFrameCount; f++)
		{
			auto& vec = ac->keyFrames[f];

			const int32 boneCount = vec.size();
			info->keyFrames[f].resize(boneCount);

			for (int32 b = 0; b < boneCount; b++)
			{
				XMVECTOR scale, rotation, translation;
				BinaryKeyFrameInfo& kf = vec[b]; // = ac->keyFrames[f][b]
				// Binary에서 파싱한 정보들로 채워준다
				KeyFrameInfo& kfInfo = info->keyFrames[f][b];
				kfInfo.boneName = kf.boneName;
				kfInfo.time = kf.time;
				XMMatrixDecompose(&scale, &rotation, &translation, kf.matTransform);
				
				kfInfo.scale = scale;
				kfInfo.rotation = rotation;
				kfInfo.translate = translation;
				
			}
		}	

		_animClips.push_back(info);
	}
#pragma endregion

#pragma region Bones
	vector<shared_ptr<BinaryBoneInfo>>& bones = loader.GetBones();
	for (shared_ptr<BinaryBoneInfo>& bone : bones)
	{
		BoneInfo boneInfo = {};
		boneInfo.matOffset = bone->matOffset;
		boneInfo.boneName = bone->boneName;
		_bones.push_back(boneInfo);
	}
#pragma endregion

#pragma region SkinData
	if (IsAnimMesh())
	{
		// BoneOffet 행렬
		const int32 boneCount = static_cast<int32>(_bones.size());
		vector<Matrix> offsetVec(boneCount);
		for (size_t b = 0; b < boneCount; b++)
			offsetVec[b] = _bones[b].matOffset;

		// OffsetMatrix StructuredBuffer 세팅
		_offsetBuffer = make_shared<StructuredBuffer>();
		_offsetBuffer->Init(sizeof(Matrix), static_cast<uint32>(offsetVec.size()), offsetVec.data());

		const int32 animCount = static_cast<int32>(_animClips.size());
		for (int32 i = 0; i < animCount; i++)
		{
			AnimClipInfo& animClip = *(_animClips[i]);

			vector<AnimFrameParams> frameParams;
			frameParams.resize(animClip.frameCount * _bones.size());

			const int32 keyFrameCount = static_cast<int32>(animClip.frameCount);
			for (int32 f = 0; f < keyFrameCount; f++)								// n번째 뼈에서 m번 프레임(시간)의 행렬들
			{																		// n번째 프레임에서 m번 뼈의 행렬들
				for (int32 b = 0; b < boneCount; b++)
				{
					int32 idx = static_cast<int32>(keyFrameCount * b + f);			// 0 / 71 / 142

					frameParams[idx] = AnimFrameParams
					{
						// Debug
						//animClip.keyFrames[f][b].boneName,
						animClip.keyFrames[f][b].scale,
						animClip.keyFrames[f][b].rotation, // Quaternion
						animClip.keyFrames[f][b].translate
					};
				}
			}

			// StructuredBuffer 세팅
			_frameBuffer.push_back(make_shared<StructuredBuffer>());
			_frameBuffer.back()->Init(sizeof(AnimFrameParams), static_cast<uint32>(frameParams.size()), frameParams.data());

		}
	}
#pragma endregion

#pragma region AnimationClip
	if (IsAnimMesh())
	{	
		// Resources로 Add한다.
		switch (type)
		{
		case PLAYER:
			GET_SINGLE(Resources)->AddAnimClip(L"Idle", _animClips[0]);
			GET_SINGLE(Resources)->AddAnimClip(L"Walk", _animClips[1]);
			GET_SINGLE(Resources)->AddAnimClip(L"Run", _animClips[2]);
			GET_SINGLE(Resources)->AddAnimClip(L"Jump", _animClips[3]);
			GET_SINGLE(Resources)->AddAnimClip(L"Death", _animClips[4]);
			GET_SINGLE(Resources)->AddAnimClip(L"Rifle_Idle", _animClips[5]);
			GET_SINGLE(Resources)->AddAnimClip(L"Rifle_Reload", _animClips[6]);
			GET_SINGLE(Resources)->AddAnimClip(L"Rifle_Shoot", _animClips[7]);
			GET_SINGLE(Resources)->AddAnimClip(L"Handgun_Idle", _animClips[8]);
			GET_SINGLE(Resources)->AddAnimClip(L"Handgun_Reload", _animClips[9]);
			GET_SINGLE(Resources)->AddAnimClip(L"Handgun_Shoot", _animClips[10]);
			GET_SINGLE(Resources)->AddAnimClip(L"Shotgun_Idle", _animClips[11]);
			GET_SINGLE(Resources)->AddAnimClip(L"Shotgun_Reload", _animClips[12]);
			GET_SINGLE(Resources)->AddAnimClip(L"Shotgun_Shoot", _animClips[13]);
			GET_SINGLE(Resources)->AddAnimClip(L"SubMachinegun_Idle", _animClips[14]);
			GET_SINGLE(Resources)->AddAnimClip(L"SubMachinegun_Reload", _animClips[15]);
			GET_SINGLE(Resources)->AddAnimClip(L"SubMachinegun_Shoot", _animClips[16]);
			GET_SINGLE(Resources)->AddAnimClip(L"OneHand_Attack", _animClips[17]);
			GET_SINGLE(Resources)->AddAnimClip(L"TwoHand_Attack", _animClips[18]);
			break;
		case ZOMBIE:
			GET_SINGLE(Resources)->AddAnimClip(L"Zombie_Idle", _animClips[0]);
			GET_SINGLE(Resources)->AddAnimClip(L"Zombie_Walk", _animClips[1]);
			break;
		}
	}
#pragma endregion
}

