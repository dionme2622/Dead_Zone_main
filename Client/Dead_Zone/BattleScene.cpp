#include "pch.h"
#include "BattleScene.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Timer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "PlayerScript.h"
#include "ZombieScript.h"
#include "CameraScript.h"
#include "Engine.h"
#include "Resources.h"
#include "MeshData.h"
#include "TestAnimation.h"
#include "ParticleSystem.h"
#include "BoxCollider.h"
#include "Animator.h"
#include "WeaponManager.h"
#include "Weapon.h"
#include "PlayerStats.h"
#include "StructuredBuffer.h"
#include <bullet3/btBulletDynamicsCommon.h>
#include "CapsuleCollider.h"
#include "RigidBody.h"
#include "PhysicsSystem.h"
// TEST
#include "KeyInput.h"
#include <random>

random_device rd;
mt19937       rng(rd());


BattleScene::BattleScene()
{

}



void BattleScene::LoadScene()
{



	_myID;
	_myID = 1;
	int _theirID = 0;
	/*if (!g_receivedMyInfo)
	{
		std::cout << "[오류] 아직 내 플레이어 정보가 서버에서 도착하지 않았습니다.\n";
		return;
	}
	if (g_myInfo.id == 0)
		_myID = g_myInfo.id + 1;
	else
		_myID = 2;
	std::cout << "[BattleScene] My ID : " << _myID << ", Position is  ("
		<< g_myInfo.x << ", " << g_myInfo.y << ", " << g_myInfo.z << ")\n";*/


	//LoadingSceneRender();
	

	// 여기서 업데이트가 실행되어야 함

#pragma region LayerMask
	SetLayerName(0, L"Battle");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region DebugCamera
	{
		//_playerCamera = make_shared<GameObject>();
		//_playerCamera->SetName(L"Debug_Camera");
		//_playerCamera->AddComponent(make_shared<Transform>());
		//_playerCamera->AddComponent(make_shared<Camera>());
		//_playerCamera->AddComponent(make_shared <TestAnimation>(_hwnd));
		//_playerCamera->GetTransform()->SetLocalPosition(Vec3(0.0f, 100.0f, 100.f));
		//_playerCamera->GetTransform()->LookAt(Vec3(0.f, 0.f, 1.f));
		//_playerCamera->GetTransform()->SetLocalRotation(Vec3(0.f, 180.f, 0.f));
		//uint8 layerIndex = LayerNameToIndex(L"UI");
		//_playerCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		//AddGameObject(_playerCamera);
	}
#pragma endregion

#pragma region SkyBox
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->SetLayerIndex(LayerNameToIndex(L"Battle"));
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(false);
		skybox->SetStatic(true);

		skybox->GetTransform()->SetLocalScale(Vec3(1.0f, 1.0f, 1.0f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Sky01", L"..\\Resources\\Texture\\SimpleSky.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		AddGameObject(skybox);
	}
#pragma endregion

#pragma region Player1
	_theirID = 1;
	bool islocal = (_theirID == _myID);

	shared_ptr<MeshData> FemaleSoldier_data = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Character\\SA_Character_FemaleSoldier.bin", PLAYER); // MeshData* meshData

	vector<shared_ptr<GameObject>> FemaleSoldier = FemaleSoldier_data->Instantiate(PLAYER, NONE);

	shared_ptr<GameObject> player1 = FemaleSoldier[23];
	for (auto& gameObject : FemaleSoldier)
	{
		//gameObject->SetName(L"FemaleSoldier");
		gameObject->SetCheckFrustum(false);
		gameObject->SetStatic(false);
		gameObject->GetTransform()->FinalUpdate();
		AddGameObject(gameObject);
	}

	player1->GetTransform()->SetLocalPosition(Vec3(-63.f, 65.f, 270.f));
	player1->AddComponent(make_shared<WeaponManager>());													// Add Weapon Manager
	player1->AddComponent(make_shared<PlayerStats>());
	player1->AddComponent(make_shared<CharacterController>(player1, 0.5, 3.0, 0.3f));
	player1->GetCharacterController()->OnEnable();
	player1->AddComponent(make_shared<PlayerScript>(_hwnd, islocal, _theirID, player1->GetCharacterController()));								// Add Player Controller
	_player.push_back(player1);
#pragma endregion

#pragma region Player2
	_theirID = 2;
	islocal = (_theirID == _myID);

	shared_ptr<MeshData> FemaleHero_data = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Character\\SA_Character_FemaleHero.bin", PLAYER); // MeshData* meshData

	vector<shared_ptr<GameObject>> FemaleHero = FemaleHero_data->Instantiate(PLAYER);

	shared_ptr<GameObject> player2 = FemaleHero[23];
	for (auto& gameObject : FemaleHero)
	{
		//gameObject->SetName(L"FemaleSoldier");
		gameObject->SetCheckFrustum(true);
		gameObject->SetStatic(false);
		gameObject->GetTransform()->FinalUpdate();
		AddGameObject(gameObject);
	}

	player2->GetTransform()->SetLocalPosition(Vec3(-58.f, 65.f, 270.f));
	//player2->AddComponent(make_shared<WeaponManager>());													// Add Weapon Manager
	player2->AddComponent(make_shared<PlayerStats>());
	player2->AddComponent(make_shared<CharacterController>(player2, 0.5, 3.0, 0.3f));
	player2->GetCharacterController()->OnEnable();
	player2->AddComponent(make_shared<PlayerScript>(_hwnd, islocal, _theirID, player2->GetCharacterController()));										// Add Weapon Manager

	_player.push_back(player2);

#pragma endregion 

#pragma region Muzzle Effect
	for (int i = 0; i < POOL_SIZE; ++i) {
		auto muzzleEffect = make_shared<GameObject>();
		muzzleEffect->SetLayerIndex(LayerNameToIndex(L"Battle")); 
		muzzleEffect->AddComponent(make_shared<Transform>());
		muzzleEffect->GetTransform()->SetLocalScale(Vec3(1000, 1000, 1000));
		muzzleEffect->GetTransform()->SetLocalPosition(Vec3(100, 100, 100));
		muzzleEffect->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"billboard");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"muzzleEffect", L"..\\Resources\\Texture\\Muzzle_Effect.png");
			shared_ptr<Material> buttonMaterials = make_shared<Material>();
			buttonMaterials->SetShader(shader);
			buttonMaterials->SetTexture(0, texture);
			meshRenderer->SetMaterial(buttonMaterials);
		}
		muzzleEffect->AddComponent(meshRenderer);
		//muzzleEffect->GetMeshRenderer()->SetRender(false);
		_muzzlePool.push_back(muzzleEffect);
		AddGameObject(muzzleEffect);
	}
#pragma endregion 

#pragma region PlayerCamera
	{
		_playerCamera = make_shared<GameObject>();
		_playerCamera->SetName(L"Main_Camera");
		_playerCamera->AddComponent(make_shared<Transform>());
		_playerCamera->AddComponent(make_shared<Camera>());
		_playerCamera->GetTransform()->SetLocalPosition(Vec3(1.2f, 3.03f, -6.65f));
		_playerCamera->GetTransform()->LookAt(Vec3(0.f, 0.f, 1.f));
		uint8 layerIndex = LayerNameToIndex(L"UI");
		_playerCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		AddGameObject(_playerCamera);
	}
	_playerCamera->GetTransform()->SetParent(_player[_myID - 1]->GetTransform());						// Player에게 Camera 를 붙인다.

#pragma endregion

#pragma region UI_Camera
	{
		_uiCamera = make_shared<GameObject>();
		_uiCamera->SetName(L"Orthographic_Camera");
		_uiCamera->AddComponent(make_shared<Transform>());
		_uiCamera->AddComponent(make_shared<Camera>());
		_uiCamera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		_uiCamera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = LayerNameToIndex(L"UI");
		_uiCamera->GetCamera()->SetCullingMaskAll(); // 다 끄고
		_uiCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 킴
		AddGameObject(_uiCamera);
	}
#pragma endregion

#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->SetCheckFrustum(false);
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		AddGameObject(obj);
	}
#pragma endregion

#pragma region ParticleSystem
	{
		/*shared_ptr<GameObject> particle = make_shared<GameObject>();
		particle->AddComponent(make_shared<Transform>());
		particle->AddComponent(make_shared<ParticleSystem>());
		particle->SetCheckFrustum(false);
		particle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 100.f));
		AddGameObject(particle);*/
	}
#pragma endregion

#pragma region Zombie

	uniform_real_distribution<float> distX(-70.0f, 0.0f);
	uniform_real_distribution<float> distZ(150.0f, 200.0f);
	{
		for (int i = 0; i < 3; ++i)
		{
			shared_ptr<MeshData> Zombie = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Zombie\\SA_Zombie_Cheerleader.bin", ZOMBIE); // MeshData* meshData

			vector<shared_ptr<GameObject>> gameObjects = Zombie->Instantiate(ZOMBIE);

			for (auto& gameObject : gameObjects)
			{
				gameObject->SetCheckFrustum(false);
				gameObject->SetStatic(false);
				AddGameObject(gameObject);
			}

			gameObjects[23]->GetTransform()->SetLocalPosition(Vec3(distX(rng), 65.0f, distZ(rng)));
			gameObjects[23]->AddComponent(make_shared<CharacterController>(gameObjects[23], 0.5, 3.0, 0.3f));
			gameObjects[23]->GetCharacterController()->SetIsPushing(false);
			gameObjects[23]->AddComponent(make_shared<PlayerStats>());
			gameObjects[23]->GetCharacterController()->OnEnable();
			gameObjects[23]->AddComponent(make_shared<ZombieScript>(_player));
			_zombies.push_back(gameObjects);
		}
	}

	{
		for (int i = 0; i < 3; ++i)
		{
			shared_ptr<MeshData> Zombie = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Zombie\\SA_Zombie_FarmersDaughter.bin", ZOMBIE); // MeshData* meshData

			vector<shared_ptr<GameObject>> gameObjects = Zombie->Instantiate(ZOMBIE);

			for (auto& gameObject : gameObjects)
			{
				gameObject->SetCheckFrustum(false);
				gameObject->SetStatic(false);
				AddGameObject(gameObject);
			}

			gameObjects[23]->GetTransform()->SetLocalPosition(Vec3(distX(rng), 65.0f, distZ(rng)));
			gameObjects[23]->AddComponent(make_shared<CharacterController>(gameObjects[23], 0.5, 3.0, 0.3f));
			gameObjects[23]->GetCharacterController()->SetIsPushing(false);
			gameObjects[23]->AddComponent(make_shared<PlayerStats>());
			gameObjects[23]->GetCharacterController()->OnEnable();
			gameObjects[23]->AddComponent(make_shared<ZombieScript>(_player));
			_zombies.push_back(gameObjects);
		}
	}

	{
		for (int i = 0; i < 3; ++i)
		{
			shared_ptr<MeshData> Zombie = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Zombie\\SA_Zombie_Farmer.bin", ZOMBIE); // MeshData* meshData

			vector<shared_ptr<GameObject>> gameObjects = Zombie->Instantiate(ZOMBIE);

			for (auto& gameObject : gameObjects)
			{
				gameObject->SetCheckFrustum(false);
				gameObject->SetStatic(false);
				AddGameObject(gameObject);
			}

			gameObjects[23]->GetTransform()->SetLocalPosition(Vec3(distX(rng), 65.0f, distZ(rng)));
			gameObjects[23]->AddComponent(make_shared<CharacterController>(gameObjects[23], 0.5, 3.0, 0.3f));
			gameObjects[23]->GetCharacterController()->SetIsPushing(false);
			gameObjects[23]->AddComponent(make_shared<PlayerStats>());
			gameObjects[23]->GetCharacterController()->OnEnable();
			gameObjects[23]->AddComponent(make_shared<ZombieScript>(_player));
			_zombies.push_back(gameObjects);
		}
	}

	{
		for (int i = 0; i < 3; ++i)
		{
			shared_ptr<MeshData> Zombie = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Zombie\\SA_Zombie_Firefighter.bin", ZOMBIE); // MeshData* meshData

			vector<shared_ptr<GameObject>> gameObjects = Zombie->Instantiate(ZOMBIE);

			for (auto& gameObject : gameObjects)
			{
				gameObject->SetCheckFrustum(false);
				gameObject->SetStatic(false);
				AddGameObject(gameObject);
			}

			gameObjects[23]->GetTransform()->SetLocalPosition(Vec3(distX(rng), 65.0f, distZ(rng)));
			gameObjects[23]->AddComponent(make_shared<CharacterController>(gameObjects[23], 0.5, 3.0, 0.3f));
			gameObjects[23]->GetCharacterController()->SetIsPushing(false);
			gameObjects[23]->AddComponent(make_shared<PlayerStats>());
			gameObjects[23]->GetCharacterController()->OnEnable();
			gameObjects[23]->AddComponent(make_shared<ZombieScript>(_player));
			_zombies.push_back(gameObjects);
		}
	}

	{
		for (int i = 0; i < 3; ++i)
		{
			shared_ptr<MeshData> Zombie = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Zombie\\SA_Zombie_FootballPlayer.bin", ZOMBIE); // MeshData* meshData

			vector<shared_ptr<GameObject>> gameObjects = Zombie->Instantiate(ZOMBIE);

			for (auto& gameObject : gameObjects)
			{
				gameObject->SetCheckFrustum(false);
				gameObject->SetStatic(false);
				AddGameObject(gameObject);
			}

			gameObjects[23]->GetTransform()->SetLocalPosition(Vec3(distX(rng), 65.0f, distZ(rng)));
			gameObjects[23]->AddComponent(make_shared<CharacterController>(gameObjects[23], 0.5, 3.0, 0.3f));
			gameObjects[23]->GetCharacterController()->SetIsPushing(false);
			gameObjects[23]->AddComponent(make_shared<PlayerStats>());
			gameObjects[23]->GetCharacterController()->OnEnable();
			gameObjects[23]->AddComponent(make_shared<ZombieScript>(_player));
			_zombies.push_back(gameObjects);
		}
	}




#pragma endregion

#pragma region Map
	{
		shared_ptr<MeshData> scene = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Map\\EnvDemo.bin"); // MeshData* meshData

		vector<shared_ptr<GameObject>> gameObjects = scene->Instantiate(OBJECT, MESH);

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetCheckFrustum(true);
			gameObject->SetStatic(false);
			AddGameObject(gameObject);
		}
	}

	{
		shared_ptr<MeshData> scene = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Map\\EnvDemo2.bin"); // MeshData* meshData

		vector<shared_ptr<GameObject>> gameObjects = scene->Instantiate(OBJECT, MESH);

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetCheckFrustum(true);
			gameObject->SetStatic(false);
			AddGameObject(gameObject);
		}
	}

	{
		shared_ptr<MeshData> scene = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Map\\BldDemo.bin"); // MeshData* meshData

		vector<shared_ptr<GameObject>> gameObjects = scene->Instantiate(OBJECT, MESH);

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetCheckFrustum(true);
			gameObject->SetStatic(false);
			AddGameObject(gameObject);
		}
	}

	{
		shared_ptr<MeshData> scene = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Map\\PropDemo.bin"); // MeshData* meshData

		vector<shared_ptr<GameObject>> gameObjects = scene->Instantiate(OBJECT, MESH);

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetCheckFrustum(true);
			gameObject->SetStatic(false);
			AddGameObject(gameObject);
		}
	}

	{
		shared_ptr<MeshData> scene = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Map\\Wall.bin"); // MeshData* meshData

		vector<shared_ptr<GameObject>> gameObjects = scene->Instantiate(OBJECT, BOX);

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetCheckFrustum(true);
			gameObject->SetStatic(false);
			AddGameObject(gameObject);
		}
	}

	{
		shared_ptr<MeshData> scene = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Kiosk_01.bin"); // MeshData* meshData

		vector<shared_ptr<GameObject>> gameObjects = scene->Instantiate(OBJECT, BOX);

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetCheckFrustum(true);
			gameObject->SetStatic(true);
			//gameObject->GetTransform()->SetLocalPosition(Vec3(57.f, 62.9, -34.5));
			AddGameObject(gameObject);
		}
	}

	{
		shared_ptr<MeshData> scene = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Kiosk_02.bin"); // MeshData* meshData

		vector<shared_ptr<GameObject>> gameObjects = scene->Instantiate(OBJECT, BOX);

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetCheckFrustum(true);
			gameObject->SetStatic(true);
			//gameObject->GetTransform()->SetLocalPosition(Vec3(57.f, 62.9, -34.5));
			AddGameObject(gameObject);
		}
	}
	{
		// 여기에 썬 오브젝트 있음
		shared_ptr<MeshData> scene = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\SkyDome.bin"); // MeshData* meshData

		//	vector<shared_ptr<GameObject>> gameObjects = scene->Instantiate(OBJECT, NONE);

		//	for (auto& gameObject : gameObjects)
		//	{
		//		gameObject->SetCheckFrustum(false);
		//		gameObject->SetStatic(true);
		//		AddGameObject(gameObject);
		//		/*if (gameObject->GetName() == L"Sun_1")
		//			_sunObject = gameObject*/;
		//	}

		//	// 이렇게 해야 맵의 중앙을 봄
		//	//_sunObject->GetTransform()->SetLocalRotation(Vec3(-45, 225, 0));
		//}

		
#pragma endregion

#pragma region Directional Light
		{
			// 태양 오브젝트 이름 : Sun_1
			_mainLight = make_shared<GameObject>();
			_mainLight->AddComponent(make_shared<Transform>());
			_mainLight->AddComponent(make_shared<Light>());
			//_mainLight->GetTransform()->SetLocalPosition(Vec3(-450.f, 950.f, -300.f));
			_mainLight->GetTransform()->SetLocalPosition(Vec3(-100.f, 250, -100));

			_mainLight->GetLight()->SetLightDirection(Vec3(0.1, -0.8, -0.4));

			_mainLight->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
			_mainLight->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));   // 밝은 흰색
			_mainLight->GetLight()->SetAmbient(Vec3(0.2f, 0.2f, 0.2f));   // 적당한 환경광
			_mainLight->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));  // 스펙큘러 강조

			_mainLight->GetLight()->SetLightIndex(0);
			AddGameObject(_mainLight);
			Vec3 look = _mainLight->GetLight()->GetTransform()->GetLook();

		}

#pragma endregion

#pragma region Spot Light
		{
			int spotLightIndex = 0;

			// 조명 위치
			array<Vec3, 9> spotLightPos = {
				Vec3(83, 72, 83),
				Vec3(59, 72, 83),
				Vec3(5, 72, 83),
				Vec3(-70, 72, 83),
				Vec3(-86, 72, 83),
				Vec3(-125, 72, 83),
				Vec3(-86.9, 72, 28.9),
				Vec3(-70.7, 72, 28.9),
				Vec3(-125.9, 72, 28.8),
			};

			for (int i = 0; i < 9; ++i)
			{
				shared_ptr<GameObject> gameObject = make_shared<GameObject>();
				gameObject->SetCheckFrustum(true);
				gameObject->SetStatic(true);
				gameObject->AddComponent(make_shared<Transform>());
				gameObject->GetTransform()->SetLocalPosition(Vec3(spotLightPos[spotLightIndex]));
				gameObject->AddComponent(make_shared<Light>());
				gameObject->GetLight()->SetLightDirection(Vec3(0, -1.0f, 0.f));
				gameObject->GetLight()->SetLightType(LIGHT_TYPE::SPOT_LIGHT);
				Vec3 pos = gameObject->GetTransform()->GetLocalPosition();
				gameObject->GetLight()->GetTransform()->SetLocalPosition(pos);

				gameObject->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
				gameObject->GetLight()->SetAmbient(Vec3(0.8f, 0.8f, 0.8f));
				gameObject->GetLight()->SetSpecular(Vec3(0.8f, 0.8f, 0.8f));
				gameObject->GetLight()->SetLightRange(20.f);
				gameObject->GetLight()->SetLightAngle(XM_PI / 1.5);
				gameObject->GetLight()->SetLightIndex(spotLightIndex + 1);
				++spotLightIndex;
				AddGameObject(gameObject);
				/*if (gameObject->GetName() == L"Sun_1")
					_sunObject = gameObject*/;
			}

			// 이렇게 해야 맵의 중앙을 봄
			//_sunObject->GetTransform()->SetLocalRotation(Vec3(-45, 225, 0));
		}
	}
#pragma endregion


	LoadUI();


	for (auto& obj : GetGameObjects())
	{
		if (auto box = obj->GetBoxCollier())
			box->UpdateWorldBounds(obj->GetTransform()->GetLocalMatrix());
	}
}

void BattleScene::Update()
{
	Scene::Update();
	GET_SINGLE(PhysicsSystem)->Update(DELTA_TIME);
	PlayerChaseShadowCamera();
	//UpdateSunOrbit();






	// TEST
	if (INPUT->GetButtonDown(KEY_TYPE::TAB))
	{

		// player1의 PlayerStats 가져오기
		auto stats = _player[0]->GetPlayerStats();
		stats->ApplyDamage(20);

		// 하트 포인트 UI 하나 제거
		if (!_heartPoint.empty())
		{
			// 하트 오브젝트를 비활성화(렌더링 끄기)
			_heartPoint.back()->GetMeshRenderer()->SetRender(false);
			_heartPoint.pop_back();
		}
	}

}

void BattleScene::LoadUI()
{

#pragma region Aiming Point
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(30.f, 30.f, 1.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0, 0, 1.f));
		obj->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"AimPoint", L"..\\Resources\\Texture\\AimPoint.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->SetCheckFrustum(false);
		obj->AddComponent(meshRenderer);
		AddGameObject(obj);
	}
#pragma endregion

#pragma region Heart Box
	{
		shared_ptr<GameObject> heart = make_shared<GameObject>();
		heart->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		heart->AddComponent(make_shared<Transform>());
		heart->GetTransform()->SetLocalScale(Vec3(427.f, 430.f, 1.f));
		heart->GetTransform()->SetLocalPosition(Vec3(-631, 450, 1.0f));
		heart->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"HeartBox", L"..\\Resources\\Texture\\HeartBox.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		heart->SetCheckFrustum(false);
		heart->AddComponent(meshRenderer);
		AddGameObject(heart);
	}
#pragma endregion

#pragma region Heart Point
	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<GameObject> heart = make_shared<GameObject>();
		heart->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		heart->AddComponent(make_shared<Transform>());
		heart->GetTransform()->SetLocalScale(Vec3(100.f, 200.f, 1.f));
		heart->GetTransform()->SetLocalPosition(Vec3(-775 + i * 32, 450, 1.0f));
		heart->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"HeartPoint", L"..\\Resources\\Texture\\HeartPoint.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		heart->SetCheckFrustum(false);
		heart->AddComponent(meshRenderer);
		_heartPoint.push_back(heart);
		AddGameObject(heart);
	}
#pragma endregion

#pragma region immunity
	{
		shared_ptr<GameObject> immunity = make_shared<GameObject>();
		immunity->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		immunity->AddComponent(make_shared<Transform>());
		immunity->GetTransform()->SetLocalScale(Vec3(120.f, 120.f, 1.f));
		immunity->GetTransform()->SetLocalPosition(Vec3(-850, 350, 1.f));
		immunity->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Immunity", L"..\\Resources\\Texture\\immunity.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		immunity->SetCheckFrustum(false);
		immunity->AddComponent(meshRenderer);
		AddGameObject(immunity);
	}
#pragma endregion

#pragma region immunity Box
	{
		shared_ptr<GameObject> immunity = make_shared<GameObject>();
		immunity->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		immunity->AddComponent(make_shared<Transform>());
		immunity->GetTransform()->SetLocalScale(Vec3(427.f, 430.f, 1.f));
		immunity->GetTransform()->SetLocalPosition(Vec3(-631, 350, 1.0f));
		immunity->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"HeartBox", L"..\\Resources\\Texture\\HeartBox.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		immunity->SetCheckFrustum(false);
		immunity->AddComponent(meshRenderer);
		AddGameObject(immunity);
	}
#pragma endregion

#pragma region immunity Point
	for (int i = 0; i < 10; ++i)
	{
		shared_ptr<GameObject> immunity = make_shared<GameObject>();
		immunity->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		immunity->AddComponent(make_shared<Transform>());
		immunity->GetTransform()->SetLocalScale(Vec3(80.f, 180.f, 1.f));
		immunity->GetTransform()->SetLocalPosition(Vec3(-775 + i * 32, 350, 1.0f));
		immunity->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"ImmunityPoint", L"..\\Resources\\Texture\\immunityPoint.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		immunity->SetCheckFrustum(false);
		immunity->AddComponent(meshRenderer);
		_immunityPoint.push_back(immunity);
		AddGameObject(immunity);
	}
#pragma endregion

#pragma region immunity
	{
		shared_ptr<GameObject> heart = make_shared<GameObject>();
		heart->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		heart->AddComponent(make_shared<Transform>());
		heart->GetTransform()->SetLocalScale(Vec3(120.f, 120.f, 1.f));
		heart->GetTransform()->SetLocalPosition(Vec3(-850, 450, 1.f));
		heart->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Heart", L"..\\Resources\\Texture\\Heart.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		heart->SetCheckFrustum(false);
		heart->AddComponent(meshRenderer);
		AddGameObject(heart);
	}
#pragma endregion
}

void BattleScene::UpdateSunOrbit()
{
	// 공전 중심 (맵 중앙)
	Vec3 center = Vec3(0.0f, 100.0f, 100.0f);

	// 공전 반지름 (고정된 거리)

	float orbitRadius = Vec3::Distance(Vec3(0, 100, 100), _sunObject->GetTransform()->GetLocalPosition()); // 중심에서 50 유닛 거리 (필요에 따라 조정)

	// 현재 회전 각도 (프레임 간 누적)
	static float currentAngle = 0.0f;
	float rotationSpeed = 20.0f; // 초당 20도
	float angle = rotationSpeed * DELTA_TIME; // 이번 프레임의 회전 각도
	currentAngle += angle; // 누적 회전 각도

	// 라디안으로 변환
	float radians = XMConvertToRadians(currentAngle);

	// 새로운 위치 계산 (xz 평면에서 원형 궤적, y축 기준 공전)
	Vec3 newPosition = center;
	newPosition.x = center.x + orbitRadius * cosf(radians); // x축 이동
	newPosition.y = _sunObject->GetTransform()->GetLocalPosition().y - 10;
	newPosition.z = center.z + orbitRadius * sinf(radians); // z축 이동
	// y축은 center.y (100.0f)로 고정

	// _sunObject의 위치 업데이트
	_sunObject->GetTransform()->SetLocalPosition(newPosition);

	// _sunObject가 중심을 바라보도록 설정 (맵 중앙: 0, 100, 100)
	_sunObject->GetTransform()->LookAt(center);
}

void BattleScene::PlayerChaseShadowCamera()
{
	Vec3 playerPos = _player[_myID - 1]->GetTransform()->GetWorldPosition();

	Vec3 lightDir = Vec3(_mainLight->GetLight()->GetLightInfo().direction.x, _mainLight->GetLight()->GetLightInfo().direction.y, _mainLight->GetLight()->GetLightInfo().direction.z);

	float shadowDistance = 100.0f;
	Vec3 lightPos = playerPos - lightDir * shadowDistance;

	_mainLight->GetTransform()->SetLocalPosition(lightPos);

	_mainLight->GetTransform()->LightLookAt(lightDir);
}

void BattleScene::LoadingSceneRender()
{
#pragma region LayerMask
	SetLayerName(0, L"Character");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma endregion Character_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Character_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>());
		camera->GetTransform()->SetLocalPosition(Vec3(0, 0, -100));
		camera->GetTransform()->LookAt(Vec3(0.f, 0.f, 1.f));
		uint8 layerIndex = LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
		camera->GetCamera()->SetNear(0.01);
		camera->GetCamera()->SetFar(1000);
		AddGameObject(camera);
	}
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>());
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, -50.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll();
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false);
		AddGameObject(camera);
	}
#pragma endregion

#pragma region Background Emage
	{
		shared_ptr<GameObject> backgroundEmage = make_shared<GameObject>();
		backgroundEmage->SetLayerIndex(LayerNameToIndex(L"Battle")); // UI
		backgroundEmage->AddComponent(make_shared<Transform>());
		backgroundEmage->GetTransform()->SetLocalScale(Vec3(1920 / 10, 1080 / 10, 1));
		backgroundEmage->GetTransform()->SetLocalPosition(Vec3(0, 0.0, 0.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Background_UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"LobbySceneBackgroundEmage", L"..\\Resources\\Texture\\LobbySceneBackground.png");
			shared_ptr<Material> backgroundMaterial = make_shared<Material>();
			backgroundMaterial->SetShader(shader);
			backgroundMaterial->SetTexture(0, texture);
			meshRenderer->SetMaterial(backgroundMaterial);
		}
		backgroundEmage->AddComponent(meshRenderer);
		AddGameObject(backgroundEmage);
	}
#pragma endregion


#pragma region Charactor Model
	{
		shared_ptr<MeshData> FemaleSoldier_data = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Character\\SA_Character_FemaleSoldier.bin", PLAYER); // MeshData* meshData

		vector<shared_ptr<GameObject>> FemaleSoldier = FemaleSoldier_data->Instantiate(PLAYER);

		shared_ptr<GameObject> characterModel = FemaleSoldier[23];
		characterModel->SetLayerIndex(LayerNameToIndex(L"Battle")); // UI

		for (auto& gameObject : FemaleSoldier)
		{
			gameObject->SetCheckFrustum(false);
			gameObject->SetStatic(false);
			gameObject->GetTransform()->FinalUpdate();

			AddGameObject(gameObject);
		}

		characterModel->GetTransform()->SetLocalPosition(Vec3(-3, -2.5, -90));
		characterModel->GetTransform()->SetLocalRotation(Vec3(0, 150, 0));
	}
#pragma endregion

#pragma region Zombie Model
	{
		shared_ptr<MeshData> zombie_data = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Character\\SA_Character_FemaleSoldier.bin", ZOMBIE); // MeshData* meshData


		vector<shared_ptr<GameObject>> gameObjects = zombie_data->Instantiate(ZOMBIE);
		
		shared_ptr<GameObject> zombieModel = gameObjects[23];
		zombieModel->SetLayerIndex(LayerNameToIndex(L"Battle"));


		for (auto& gameObject : gameObjects)
		{
			gameObject->SetCheckFrustum(false);
			gameObject->SetStatic(false);
			gameObject->GetTransform()->FinalUpdate();

			AddGameObject(gameObject);
		}
		
		zombieModel->GetTransform()->SetLocalPosition(Vec3(-8, -2.5, -85));
		zombieModel->GetTransform()->SetLocalRotation(Vec3(0, 150, 0));
	}
#pragma endregion

#pragma region Loading Text
	{
		shared_ptr<GameObject> title = make_shared<GameObject>();
		title->SetLayerIndex(LayerNameToIndex(L"UI"));
		title->AddComponent(make_shared<Transform>());
		title->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 2, WINDOW.height / 2, 1));
		title->GetTransform()->SetLocalPosition(Vec3(400, -350.0, 1.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"LoadingText", L"..\\Resources\\Texture\\Loading_Text.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		title->AddComponent(meshRenderer);
		AddGameObject(title);
	}
#pragma endregion


#pragma region Light
	shared_ptr<GameObject> light = make_shared<GameObject>();
	light->AddComponent(make_shared<Transform>());
	light->AddComponent(make_shared<Light>());
	light->GetTransform()->SetLocalPosition(Vec3(10, 10, -10));

	light->GetLight()->SetLightDirection(Vec3(-1, -1, 0.5));

	light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));   // 밝은 흰색
	light->GetLight()->SetAmbient(Vec3(0.2f, 0.2f, 0.2f));   // 적당한 환경광
	light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));  // 스펙큘러 강조
	AddGameObject(light);
#pragma endregion

}

void BattleScene::ShowMuzzleEffect(const Vec3& pos) {
	for (auto& muzzle : _muzzlePool) {
		/*if (!muzzle->GetMeshRenderer()->IsRender())*/ {

			muzzle->GetTransform()->SetLocalPosition(pos);
			muzzle->GetMeshRenderer()->SetRender(true);
			// 타이머 등으로 일정 시간 후 SetRender(false)
			break;
		}
	}
}