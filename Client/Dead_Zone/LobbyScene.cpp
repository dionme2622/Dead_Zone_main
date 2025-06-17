#include "pch.h"
#include "LobbyScene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "PlayerStats.h"
#include "Resources.h"
#include "KeyInput.h"
#include "SceneManager.h"
#include "MeshData.h"
#include "WeaponManager.h"
#include "Timer.h"
#include "Light.h"

void LobbyScene::LoadScene()
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

#pragma region Charactor Model
	{
		shared_ptr<MeshData> FemaleSoldier_data = GET_SINGLE(Resources)->LoadModelFromBinary(L"..\\Resources\\Model\\Character\\SA_Character_FemaleSoldier.bin", PLAYER); // MeshData* meshData

		vector<shared_ptr<GameObject>> FemaleSoldier = FemaleSoldier_data->Instantiate(PLAYER, NONE);

		shared_ptr<GameObject> characterModel = FemaleSoldier[23];
		characterModel->SetLayerIndex(LayerNameToIndex(L"Character")); // UI

		for (auto& gameObject : FemaleSoldier)
		{
			gameObject->SetName(L"FemaleSoldier");
			gameObject->SetCheckFrustum(false);
			gameObject->SetStatic(false);
			gameObject->GetTransform()->FinalUpdate();

			AddGameObject(gameObject);
		}

		characterModel->GetTransform()->SetLocalPosition(Vec3(-1.5, -2, -96));
		characterModel->GetTransform()->SetLocalRotation(Vec3(0, 170, 0));
	}
#pragma endregion



#pragma region Background Emage
	{
		shared_ptr<GameObject> backgroundEmage = make_shared<GameObject>();
		backgroundEmage->SetLayerIndex(LayerNameToIndex(L"Character")); // UI
		backgroundEmage->AddComponent(make_shared<Transform>());
		backgroundEmage->GetTransform()->SetLocalScale(Vec3(1920 / 12, 1020 / 12, 1));
		backgroundEmage->GetTransform()->SetLocalPosition(Vec3(0, 0.0, 0.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Background_UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"LobbySceneBackgroundEmage", L"..\\Resources\\Texture\\LobbySceneBackground.png");
			_backgroundMaterial = make_shared<Material>();
			_backgroundMaterial->SetShader(shader);
			_backgroundMaterial->SetTexture(0, texture);
			meshRenderer->SetMaterial(_backgroundMaterial);
		}
		backgroundEmage->AddComponent(meshRenderer);
		AddGameObject(backgroundEmage);
	}
#pragma endregion

#pragma region Title
	{
		shared_ptr<GameObject> title = make_shared<GameObject>();
		title->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		title->AddComponent(make_shared<Transform>());
		title->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 2, WINDOW.height / 2, 1));
		title->GetTransform()->SetLocalPosition(Vec3(400, 100.0, 1.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Title", L"..\\Resources\\Texture\\Title.png");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetInt(1, 1);
			meshRenderer->SetMaterial(material);
		}
		title->AddComponent(meshRenderer);
		AddGameObject(title);
	}
#pragma endregion


#pragma region Login Button
	{
		shared_ptr<GameObject> loginButton = make_shared<GameObject>();
		loginButton->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		loginButton->AddComponent(make_shared<Transform>());
		loginButton->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 5, WINDOW.height / 5, 1));
		loginButton->GetTransform()->SetLocalPosition(Vec3(600, -300, 1.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Button_UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"LoginButton", L"..\\Resources\\Texture\\LoginButton.png");
			_ButtonMaterials[0] = make_shared<Material>();
			_ButtonMaterials[0]->SetShader(shader);
			_ButtonMaterials[0]->SetTexture(0, texture);
			meshRenderer->SetMaterial(_ButtonMaterials[0]);
		}
		loginButton->AddComponent(meshRenderer);
		AddGameObject(loginButton);
		_buttons.push_back(loginButton);
	}
#pragma endregion


#pragma region Quit Button
	{
		shared_ptr<GameObject> QuitButton = make_shared<GameObject>();
		QuitButton->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		QuitButton->AddComponent(make_shared<Transform>());
		QuitButton->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 5, WINDOW.height / 5, 1));
		QuitButton->GetTransform()->SetLocalPosition(Vec3(750, 400, 1.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"QuitButton", L"..\\Resources\\Texture\\QuitButton.png");
			_ButtonMaterials[1] = make_shared<Material>();
			_ButtonMaterials[1]->SetShader(shader);
			_ButtonMaterials[1]->SetTexture(0, texture);
			meshRenderer->SetMaterial(_ButtonMaterials[1]);
		}
		QuitButton->AddComponent(meshRenderer);
		AddGameObject(QuitButton);
		_buttons.push_back(QuitButton);

	}
#pragma endregion

#pragma region Id Box
	{
		shared_ptr<GameObject> idBox = make_shared<GameObject>();
		idBox->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		idBox->AddComponent(make_shared<Transform>());
		idBox->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 6, WINDOW.height / 12, 1));
		idBox->GetTransform()->SetLocalPosition(Vec3(300, -300, 1.f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Transparent_UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"idBox", L"..\\Resources\\Texture\\Clear_box.png");
			shared_ptr<Material> buttonMaterials = make_shared<Material>();
			buttonMaterials->SetShader(shader);
			buttonMaterials->SetTexture(0, texture);
			meshRenderer->SetMaterial(buttonMaterials);
		}
		idBox->AddComponent(meshRenderer);
		AddGameObject(idBox);
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

void LobbyScene::Update()
{
	Scene::Update();
	
	UpdateButton();

	HandleIdInput();
}

void LobbyScene::UpdateButton()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(WINDOW.hwnd, &pt);

	for (size_t i = 0; i < _buttons.size(); ++i)
	{
		auto& button = _buttons[i];
		Vec3 btnPos = button->GetTransform()->GetLocalPosition();
		Vec3 btnScale = button->GetTransform()->GetLocalScale();

		float btnW = btnScale.x / 2.0f;
		float btnH = btnScale.y / 2.0f;

		float btnX = (WINDOW.width / 2.0f) + btnPos.x - btnW;
		float btnY = (WINDOW.height / 2.0f) - btnPos.y - btnH;

		bool isHover =
			(pt.x >= btnX && pt.x <= btnX + btnScale.x) &&
			(pt.y >= btnY && pt.y <= btnY + btnScale.y);

		if (i == 0) // Login Button
		{
			if (!isHover)
				_ButtonMaterials[0]->SetInt(1, 0); // Normal state
			else
			{
				if (INPUT->GetButton(KEY_TYPE::LEFTCLICK))
					_ButtonMaterials[0]->SetInt(1, 0); // Pressed state
				else if (INPUT->GetButtonUp(KEY_TYPE::LEFTCLICK))
				{
					_ButtonMaterials[0]->SetInt(1, 1); // Hover state

					// 연결 성공
					{
						// 여기 서버 연결 (연결 성공시에만 다음 씬 이동)
						// _inputId(아이디)
						GET_SINGLE(SceneManager)->LoadScene(SCENETYPE::ESELECTROOMSCENE);
					}
					// 연결 실패
					// return 
				}
				else
					_ButtonMaterials[0]->SetInt(1, 1); // Hover state
			}
		}
		else if (i == 1) // Quit Button
		{
			if (!isHover)
				_ButtonMaterials[1]->SetInt(1, 0); // Normal state
			else
			{
				if (INPUT->GetButton(KEY_TYPE::LEFTCLICK))
					_ButtonMaterials[1]->SetInt(1, 0); 
				else if (INPUT->GetButtonUp(KEY_TYPE::LEFTCLICK))
				{
					_ButtonMaterials[1]->SetInt(1, 1); 
					PostQuitMessage(0);
				}
				else
					_ButtonMaterials[1]->SetInt(1, 1); // Hover state
			}
		}
	}
}

void LobbyScene::HandleIdInput()
{
	for (int key = '0'; key <= '9'; ++key)
	{
		if (INPUT->GetButtonDown(static_cast<KEY_TYPE>(key)))
		{
			if (_inputId.length() < 16) 
				_inputId += static_cast<char>(key);
		}
	}
	for (int key = 'A'; key <= 'Z'; ++key)
	{
		if (INPUT->GetButtonDown(static_cast<KEY_TYPE>(key)))
		{
			if (_inputId.length() < 16)
				_inputId += static_cast<char>(key);
		}
	}
	for (int key = 'a'; key <= 'z'; ++key)
	{
		if (INPUT->GetButtonDown(static_cast<KEY_TYPE>(key)))
		{
			if (_inputId.length() < 16)
				_inputId += static_cast<char>(key);
		}
	}

	if (INPUT->GetButtonDown(KEY_TYPE::BACK))
	{
		if (!_inputId.empty())
			_inputId.pop_back();
	}

	cout << _inputId << endl;
}




