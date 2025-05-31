#include "pch.h"
#include "Resources.h"
#include "Engine.h"
#include "MeshData.h"
#include "AnimatorController.h"
#include "AnimationState.h"

void Resources::Init()
{
	CreateDefaultShader();
	CreateDefaultMaterial();
}

shared_ptr<Mesh> Resources::LoadPointMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Point");
	if (findMesh)
		return findMesh;

	vector<Vertex> vec(1);
	vec[0] = Vertex(Vec3(0, 0, 0), Vec2(0.5f, 0.5f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(1);
	idx[0] = 0;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Init(vec, idx);
	Add(L"Point", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadRectangleMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Rectangle");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;

	vector<Vertex> vec(4);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, 0), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, 0), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, 0), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, 0), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

	vector<uint32> idx(6);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Init(vec, idx);
	Add(L"Rectangle", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadCubeMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Cube");
	if (findMesh)
		return findMesh;

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	vector<Vertex> vec(24);

	// 앞면
	vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 뒷면
	vec[4] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[5] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[6] = Vertex(Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[7] = Vertex(Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 윗면
	vec[8] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[9] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[10] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[11] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	// 아랫면
	vec[12] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[13] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[14] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	vec[15] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	// 왼쪽면
	vec[16] = Vertex(Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[17] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[18] = Vertex(Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	vec[19] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	// 오른쪽면
	vec[20] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[21] = Vertex(Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[22] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	vec[23] = Vertex(Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

	vector<uint32> idx(36);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// 뒷면
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// 윗면
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// 아랫면
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// 왼쪽면
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// 오른쪽면
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Init(vec, idx);
	Add(L"Cube", mesh);

	return mesh;
}

shared_ptr<Mesh> Resources::LoadSphereMesh()
{
	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Sphere");
	if (findMesh)
		return findMesh;

	float radius = 0.5f; // 구의 반지름
	uint32 stackCount = 20; // 가로 분할
	uint32 sliceCount = 20; // 세로 분할

	vector<Vertex> vec;

	Vertex v;

	// 북극
	v.pos = Vec3(0.0f, radius, 0.0f);
	v.uv = Vec2(0.5f, 0.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 1.0f);
	vec.push_back(v);

	float stackAngle = XM_PI / stackCount;
	float sliceAngle = XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
	for (uint32 y = 1; y <= stackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// 고리에 위치한 정점
		for (uint32 x = 0; x <= sliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.pos;
			v.normal.Normalize();

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent.Normalize();

			vec.push_back(v);
		}
	}

	// 남극
	v.pos = Vec3(0.0f, -radius, 0.0f);
	v.uv = Vec2(0.5f, 1.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = Vec3(1.0f, 0.0f, 0.0f);
	vec.push_back(v);

	vector<uint32> idx(36);

	// 북극 인덱스
	for (uint32 i = 0; i <= sliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// 몸통 인덱스
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; ++y)
	{
		for (uint32 x = 0; x < sliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// 남극 인덱스
	uint32 bottomIndex = static_cast<uint32>(vec.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Init(vec, idx);
	Add(L"Sphere", mesh);

	return mesh;
}

shared_ptr<MeshData> Resources::LoadModelFromBinary(const wstring& path, int type)
{
	wstring key = path;

	//shared_ptr<MeshData> meshData = Get<MeshData>(key);
	//if (meshData)
	//	return meshData;

	// TODO: wstring -> char
	const string& spath = ws2s(path);
	const char* Filepath = spath.c_str();

	shared_ptr<MeshData> meshData = MeshData::LoadModelFromBinary(Filepath, type);
	//meshData->SetName(key);
	//Add(key, meshData);

	return meshData;
}

shared_ptr<AnimatorController> Resources::LoadAnimatorPlayerController()
{
	// 1) Controller 생성
	auto controller = make_shared<AnimatorController>();

	// 2) 파라미터 정의: Speed (float)
	AnimatorParameter speedParam{ "Speed", ParameterType::Float , 0.1f };
	controller->AddParameter(speedParam);

	// 2) 파라미터 정의: isJumping (bool)
	AnimatorParameter isJumpingParam{ "isJumping" , ParameterType::Bool, 0.0 };
	controller->AddParameter(isJumpingParam);

	// 3) 스테이트 생성 (이름, 클립, 클립 인덱스, 속도, loop)
	auto Idle = make_shared<AnimationState>(L"Idle", GetAnimClip(L"Idle"), 0, 1.0f, true);
	auto Walk = make_shared<AnimationState>(L"Walk", GetAnimClip(L"Walk"), 1, 1.0f, true);
	auto Run = make_shared<AnimationState>(L"Run", GetAnimClip(L"Run"), 2, 1.0f, true);
	auto Jump = make_shared<AnimationState>(L"Jump", GetAnimClip(L"Jump"), 3, 1.0f, true);
	auto Death = make_shared<AnimationState>(L"Death", GetAnimClip(L"Death"), 4, 1.0f, true);
	auto Rifle_Idle = make_shared<AnimationState>(L"Rifle_Idle", GetAnimClip(L"Rifle_Idle"), 5, 1.0f, true);
	auto Rifle_Reload = make_shared<AnimationState>(L"Rifle_Reload", GetAnimClip(L"Rifle_Reload"), 6, 1.0f, true);
	auto Rifle_Shoot = make_shared<AnimationState>(L"Rifle_Shoot", GetAnimClip(L"Rifle_Shoot"), 7, 1.0f, true);
	auto Handgun_Idle = make_shared<AnimationState>(L"Handgun_Idle", GetAnimClip(L"Handgun_Idle"), 8, 1.0f, true);
	auto Handgun_Reload = make_shared<AnimationState>(L"Handgun_Reload", GetAnimClip(L"Handgun_Reload"), 9, 1.0f, true);
	auto Handgun_Shoot = make_shared<AnimationState>(L"Handgun_Shoot", GetAnimClip(L"Handgun_Shoot"), 10, 1.0f, true);
	auto Shotgun_Idle = make_shared<AnimationState>(L"Shotgun_Idle", GetAnimClip(L"Shotgun_Idle"), 11, 1.0f, true);
	auto Shotgun_Reload = make_shared<AnimationState>(L"Shotgun_Reload", GetAnimClip(L"Shotgun_Reload"), 12, 1.0f, true);
	auto Shotgun_Shoot = make_shared<AnimationState>(L"Shotgun_Shoot", GetAnimClip(L"Shotgun_Shoot"), 13, 1.0f, true);
	auto SubMachinegun_Idle = make_shared<AnimationState>(L"SubMachinegun_Idle", GetAnimClip(L"SubMachinegun_Idle"), 14, 1.0f, true);
	auto SubMachinegun_Reload = make_shared<AnimationState>(L"SubMachinegun_Reload", GetAnimClip(L"SubMachinegun_Reload"), 15, 1.0f, true);
	auto SubMachinegun_Shoot = make_shared<AnimationState>(L"SubMachinegun_Shoot", GetAnimClip(L"SubMachinegun_Shoot"), 16, 1.0f, true);
	auto OneHand_Attack = make_shared<AnimationState>(L"OneHand_Attack", GetAnimClip(L"OneHand_Attack"), 17, 1.0f, true);
	auto TwoHand_Attack = make_shared<AnimationState>(L"TwoHand_Attack", GetAnimClip(L"TwoHand_Attack"), 18, 1.0f, true);
	controller->AddState(Idle);
	controller->AddState(Walk);
	controller->AddState(Run);
	controller->AddState(Jump);

	controller->AddState(Rifle_Shoot);

	int speedIdx = controller->GetParamIndex("Speed");
	int isJumpingIdx = controller->GetParamIndex("isJumping");

	// 4) Idle → Walk 전이 추가 (Speed > 0.1)
	{
		auto t = make_shared<Transition>(Walk);
		t->AddCondition(
			/*paramIndex=*/speedIdx,
			ParameterType::Float,
			ConditionMode::Greater,
			/*threshold=*/4.0f,
			/*exitTime=*/0.0f,
			/*duration=*/0.0f
		);
		Idle->AddTransition(t);
	}

	// 5) Walk → Idle 전이 추가 (Speed < 0.05)
	{
		auto t = make_shared<Transition>(Run);
		t->AddCondition(
			/*paramIndex=*/speedIdx,
			ParameterType::Float,
			ConditionMode::Greater,
			/*threshold=*/8.0f,
			/*exitTime=*/0.0f,
			/*duration=*/0.0f
		);
		Walk->AddTransition(t);
	}
	{
		auto t = make_shared<Transition>(Idle);
		t->AddCondition(
			/*paramIndex=*/speedIdx,
			ParameterType::Float,
			ConditionMode::Less,
			/*threshold=*/4.0f,
			/*exitTime=*/0.0f,
			/*duration=*/0.0f
		);
		Walk->AddTransition(t);
	}
	{
		auto t = make_shared<Transition>(Walk);
		t->AddCondition(
			/*paramIndex=*/speedIdx,
			ParameterType::Float,
			ConditionMode::Less,
			/*threshold=*/8.0f,
			/*exitTime=*/0.0f,
			/*duration=*/0.0f
		);
		Run->AddTransition(t);
	}

	// □ Idle/Walk/Run → Jump
	for (auto baseState : { Idle, Walk, Run })
	{
		auto t = make_shared<Transition>(Jump);
		// isJumping == true 일 때 전환
		t->AddCondition(
			/*paramIndex=*/ isJumpingIdx,
			ParameterType::Bool,
			/*mode=*/       ConditionMode::Equals,
			/*threshold=*/  1.0f,       // Bool:true → 1.0
			/*exitTime=*/   0.0f,
			/*duration=*/   0.1f        // 부드러운 페이드 타임
		);
		baseState->AddTransition(t);
	}

	// □ Jump → Idle (착지 시 자동 복귀)
	{
		auto t = make_shared<Transition>(Idle);
		// isJumping == false 일 때 전환
		t->AddCondition(
			/*paramIndex=*/ isJumpingIdx,
			ParameterType::Bool,
			/*mode=*/       ConditionMode::Equals,
			/*threshold=*/  0.0f,       // Bool:false → 0.0
			/*exitTime=*/   0.0f,
			/*duration=*/   0.1f
		);
		Jump->AddTransition(t);
	}

	// 4) 클릭 시 Shoot 상태로 전이
	//for (auto baseState : { Idle, Walk, Run })
	//{
	//	auto t = make_shared<Transition>(Rifle_Shoot);
	//	t->AddCondition(shootIdx, ConditionMode::Equals, 1.0f, /*exitTime=*/0.0f, /*fade=*/0.1f);
	//	baseState->AddTransition(t);
	//}

	//// 5) Shoot 끝나면 Idle로 복귀 (exitTime=1.0f)
	//{
	//	auto t = make_shared<Transition>(Idle);
	//	t->AddCondition(shootIdx, ConditionMode::Equals, 1.0f, /*exitTime=*/0.0f, /*fade=*/0.1f);
	//	Rifle_Shoot->AddTransition(t);
	//}


	// 6) Entry State 설정
	controller->SetEntryState(L"Idle");

	return controller;
}

shared_ptr<class AnimatorController> Resources::LoadAnimatorZombieController()
{
	// 1) Controller 생성
	auto controller = make_shared<AnimatorController>();

	// 2) 파라미터 정의: isWalkingParam (bool)
	AnimatorParameter isWalkingParam{ "isWalking" , ParameterType::Bool, 1.0 };
	controller->AddParameter(isWalkingParam);

	// 3) 스테이트 생성 (이름, 클립, 클립 인덱스, 속도, loop)
	auto Idle = make_shared<AnimationState>(L"Zombie_Idle", GetAnimClip(L"Zombie_Idle"), 0, 1.0f, true);
	auto Walk = make_shared<AnimationState>(L"Zombie_Walk", GetAnimClip(L"Zombie_Walk"), 1, 1.0f, true);

	controller->AddState(Idle);
	controller->AddState(Walk);
	int isWalkingIdx = controller->GetParamIndex("isWalking");
	// 4) Idle → Walk 전이 추가 (Speed > 0.1)
	{
		auto t = make_shared<Transition>(Walk);
		t->AddCondition(
			/*paramIndex=*/isWalkingIdx,
			ParameterType::Bool,
			/*mode=*/       ConditionMode::Equals,
			/*threshold=*/  1.0f,       // Bool:false → 0.0
			/*exitTime=*/   0.0f,
			/*duration=*/   0.1f
		);
		Idle->AddTransition(t);
	}

	// 5) Walk → Idle 전이 추가 (Speed < 0.05)
	{
		auto t = make_shared<Transition>(Idle);
		t->AddCondition(
			/*paramIndex=*/isWalkingIdx,
			ParameterType::Bool,
			/*mode=*/       ConditionMode::Equals,
			/*threshold=*/  0.0f,       // Bool:false → 0.0
			/*exitTime=*/   0.0f,
			/*duration=*/   0.1f
		);
		Walk->AddTransition(t);
	}


	// 6) Entry State 설정
	controller->SetEntryState(L"Zombie_Idle");
	return controller;
}

//void Resources::LoadSceneFromBinary(const wstring& path)
//{
//	wstring name;
//	for (; ;)		// 객체 수 만큼
//	{
//		LoadModelFromBinary(L"..\\Resources\\Model\\" + name + L".bin");
//	}
//}

shared_ptr<Texture> Resources::CreateTexture(const wstring& name, DXGI_FORMAT format, uint64 width, uint64 height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
	D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->Create(format, width, height, heapProperty, heapFlags, resFlags, clearColor);
	Add(name, texture);

	return texture;
}

shared_ptr<Texture> Resources::CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource>& tex2D)
{
	shared_ptr<Texture> texture = make_shared<Texture>();
	texture->CreateFromResource(tex2D);
	Add(name, texture);

	return texture;
}

void Resources::AddAnimClip(const wstring& key, shared_ptr<AnimClipInfo> clip)
{
	_animClips[key] = clip;
}

shared_ptr<AnimClipInfo> Resources::GetAnimClip(const wstring& key) const
{
	auto it = _animClips.find(key);
	if (it != _animClips.end())
		return it->second;
	return nullptr;
}


void Resources::CreateDefaultShader()
{
	// Skybox
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS_EQUAL
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\skybox.fx", info);
		Add<Shader>(L"Skybox", shader);
	}

	// Deferred (Deferred)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::CULL_BACK
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\deferred.fx", info);
		Add<Shader>(L"Deferred", shader);
	}

	// Deferred Wireframe (Deferred)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::WIREFRAME
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\deferred.fx", info);
		Add<Shader>(L"Deferred_Wire", shader);
	}

	// Forward (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info);
		Add<Shader>(L"Forward", shader);
	}

	// Texture (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE
		};

		ShaderArg arg =
		{
			"VS_Tex",
			"",
			"",
			"",
			"PS_Tex"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info, arg);
		Add<Shader>(L"Texture", shader);
	}

	// UI
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
		};

		ShaderArg arg =
		{
			"VS_UI",
			"",
			"",
			"",
			"PS_UI"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info, arg);
		Add<Shader>(L"UI", shader);
	}

	// DirLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_DirLight",
			"",
			"",
			"",
			"PS_DirLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"DirLight", shader);
	}

	// SpotLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_SpotLight",
			"",
			"",
			"",
			"PS_SpotLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"SpotLight", shader);
	}

	// PointLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		ShaderArg arg =
		{
			"VS_PointLight",
			"",
			"",
			"",
			"PS_PointLight"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"PointLight", shader);
	}

	// Final
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
		};

		ShaderArg arg =
		{
			"VS_Final",
			"",
			"",
			"",
			"PS_Final"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\lighting.fx", info, arg);
		Add<Shader>(L"Final", shader);
	}

	// Shadow
	{
		ShaderInfo info =
		{
			SHADER_TYPE::SHADOW,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\shadow.fx", info);
		Add<Shader>(L"Shadow", shader);
	}

	//// Compute Shader
	//{
	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	shader->CreateComputeShader(L"..\\Resources\\Shader\\compute.fx", "CS_Main", "cs_5_0");
	//	Add<Shader>(L"ComputeShader", shader);
	//}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\animation.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeAnimation", shader);
	}
	// Particle
	{
		ShaderInfo info =
		{
			SHADER_TYPE::PARTICLE,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS_NO_WRITE,
			BLEND_TYPE::ALPHA_BLEND,
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"",
			"",
			"GS_Main",
			"PS_Main"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\particle.fx", info, arg);
		Add<Shader>(L"Particle", shader);
	}

	// ComputeParticle
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\particle.fx", "CS_Main", "cs_5_0");
		Add<Shader>(L"ComputeParticle", shader);
	}

	// Collider
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_LINELIST
		};

		ShaderArg arg =
		{
			"VS_Collider",
			"",
			"",
			"",
			"PS_Collider"
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\forward.fx", info, arg);
		Add<Shader>(L"Collider", shader);
	}


	//// Final
	//{
	//	ShaderInfo info =
	//	{
	//		SHADER_TYPE::POSTPROCCESS,
	//		RASTERIZER_TYPE::CULL_NONE,
	//		DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
	//	};

	//	ShaderArg arg =
	//	{
	//		"VS_PostProccessing",
	//		"",
	//		"",
	//		"",
	//		"PS_PostProccessing"
	//	};

	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	shader->CreateGraphicsShader(L"..\\Resources\\Shader\\postprocess.fx", info, arg);
	//	Add<Shader>(L"PostProcess", shader);
	//}


	//// SwapChain
	//{
	//	ShaderInfo info =
	//	{
	//		SHADER_TYPE::DEFERRED,
	//		RASTERIZER_TYPE::CULL_NONE,
	//		DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
	//	};

	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	shader->CreateGraphicsShader(L"..\\Resources\\Shader\\swapchain.fx", info, "VS_Main", "PS_Main");
	//	Add<Shader>(L"SwapChain", shader);
	//}

	//// Compute SwapChain
	//{
	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	shader->CreateComputeShader(L"..\\Resources\\Shader\\swapchain.fx", "CS_Main", "cs_5_0");
	//	Add<Shader>(L"ComputeSwapChain", shader);
	//}
}

	

void Resources::CreateDefaultMaterial()
{
	// Skybox
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Skybox", material);
	}

	// DirLight
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"DirLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		Add<Material>(L"DirLight", material);
	}

	// SpotLight
	{
		const WindowInfo& window = GEngine->GetWindow();
		Vec2 resolution = { static_cast<float>(window.width), static_cast<float>(window.height) };

		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"SpotLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		material->SetVec2(0, resolution);
		Add<Material>(L"SpotLight", material);
	}

	// PointLight
	{
		const WindowInfo& window = GEngine->GetWindow();
		Vec2 resolution = { static_cast<float>(window.width), static_cast<float>(window.height) };

		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"PointLight");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"PositionTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"NormalTarget"));
		material->SetVec2(0, resolution);
		Add<Material>(L"PointLight", material);
	}

	// Final
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Final");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseTarget"));
		material->SetTexture(1, GET_SINGLE(Resources)->Get<Texture>(L"DiffuseLightTarget"));
		material->SetTexture(2, GET_SINGLE(Resources)->Get<Texture>(L"SpecularLightTarget"));
		material->SetTexture(3, GET_SINGLE(Resources)->Get<Texture>(L"MetallicLightTarget"));
		Add<Material>(L"Final", material);
	}

	// Shadow
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Shadow");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Shadow", material);
	}

	//// Compute Shader
	//{
	//	shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");
	//	shared_ptr<Material> material = make_shared<Material>();
	//	material->SetShader(shader);
	//	Add<Material>(L"ComputeShader", material);
	//}

	// Particle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Particle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"Particle", material);
	}

	// ComputeParticle
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeParticle");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeParticle", material);
	}

	// ComputeAnimation
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeAnimation");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);

		Add<Material>(L"ComputeAnimation", material);
	}

	// Weapon
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Deferred");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetInt(2, 1);
		material->SetShader(shader);

		Add<Material>(L"Weapon", material);
	}
}