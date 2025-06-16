#include "pch.h"
#include "RoomSelectScene.h"
#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Resources.h"
#include "KeyInput.h"
#include "SceneManager.h"


void RoomSelectScene::LoadScene()
{
#pragma region LayerMask
	SetLayerName(0, L"UI");
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>());
		//camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, -50.f));
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
		backgroundEmage->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		backgroundEmage->AddComponent(make_shared<Transform>());
		backgroundEmage->GetTransform()->SetLocalScale(Vec3(WINDOW.width, WINDOW.height, 1));
		backgroundEmage->GetTransform()->SetLocalPosition(Vec3(0, 0.0, 1.f));

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

#pragma region Room Box
	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject> roomBox = make_shared<GameObject>();
		roomBox->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		roomBox->AddComponent(make_shared<Transform>());
		roomBox->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 2, WINDOW.height / 9, 1));
		roomBox->GetTransform()->SetLocalPosition(Vec3(-400, 300 - i * 200, 0.5f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Transparent_UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"roomBox", L"..\\Resources\\Texture\\Clear_box.png");
			shared_ptr<Material> buttonMaterials = make_shared<Material>();
			buttonMaterials->SetShader(shader);
			buttonMaterials->SetTexture(0, texture);
			meshRenderer->SetMaterial(buttonMaterials);
		}
		roomBox->AddComponent(meshRenderer);
		AddGameObject(roomBox);
		_roomBoxes.push_back(roomBox);
	}

	for (int i = 0; i < 3; ++i)
	{
		shared_ptr<GameObject> roomBox = make_shared<GameObject>();
		roomBox->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		roomBox->AddComponent(make_shared<Transform>());
		roomBox->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 6, WINDOW.height / 6, 1));
		roomBox->GetTransform()->SetLocalPosition(Vec3(-680, 290 - i * 200, 0.5f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");

			wstring texKey = L"room" + to_wstring(i + 1);
			wstring texPath = L"..\\Resources\\Texture\\Room" + to_wstring(i + 1) + L".png";

			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(texKey, texPath);
			shared_ptr<Material> buttonMaterials = make_shared<Material>();
			buttonMaterials->SetShader(shader);
			buttonMaterials->SetTexture(0, texture);
			meshRenderer->SetMaterial(buttonMaterials);
		}
		roomBox->AddComponent(meshRenderer);
		AddGameObject(roomBox);
	}
#pragma endregion

#pragma region Arrow Marking
	{
		_arrowMark = make_shared<GameObject>();
		_arrowMark->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		_arrowMark->AddComponent(make_shared<Transform>());
		_arrowMark->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 12, WINDOW.height / 12, 1));
		_arrowMark->GetTransform()->SetLocalPosition(Vec3(0, 0, 0.5f));


		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"ArrowMark", L"..\\Resources\\Texture\\Arrow_Texture.png");
			shared_ptr<Material> buttonMaterials = make_shared<Material>();
			buttonMaterials->SetShader(shader);
			buttonMaterials->SetTexture(0, texture);
			meshRenderer->SetMaterial(buttonMaterials);

		}
		_arrowMark->AddComponent(meshRenderer);
		_arrowMark->GetMeshRenderer()->SetRender(false);
		AddGameObject(_arrowMark);
	}


#pragma region Quit Button
	{
		shared_ptr<GameObject> QuitButton = make_shared<GameObject>();
		QuitButton->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
		QuitButton->AddComponent(make_shared<Transform>());
		QuitButton->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 3, WINDOW.height / 3, 1));
		QuitButton->GetTransform()->SetLocalPosition(Vec3(-650, -300, 0.5f));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
			shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Refresh_Button", L"..\\Resources\\Texture\\REFRESH_BUTTON.png");
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
	
	// number Test
	//{
	//	shared_ptr<GameObject> number = make_shared<GameObject>();
	//	number->SetLayerIndex(LayerNameToIndex(L"UI")); // UI
	//	number->AddComponent(make_shared<Transform>());
	//	number->GetTransform()->SetLocalScale(Vec3(WINDOW.width / 4, WINDOW.height / 4, 1));
	//	number->GetTransform()->SetLocalPosition(Vec3(400, 100.0, 1.f));

	//	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	//	{
	//		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
	//		meshRenderer->SetMesh(mesh);
	//	}
	//	{
	//		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Number_UI");
	//		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"number", L"..\\Resources\\Texture\\Number_Texture.png");
	//		shared_ptr<Material> material = make_shared<Material>();
	//		material->SetShader(shader);
	//		material->SetTexture(0, texture);
	//		meshRenderer->SetMaterial(material);
	//	}
	//	number->AddComponent(meshRenderer);
	//	AddGameObject(number);
	//}
}

void RoomSelectScene::Update()
{
	Scene::Update();

	UpdateArrowPosition();
}

void RoomSelectScene::UpdateArrowPosition()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(WINDOW.hwnd, &pt);

	_hoveredRoomIndex = -1;

	for (int i = 0; i < _roomBoxes.size(); ++i)
	{
		auto& box = _roomBoxes[i];
		Vec3 pos = box->GetTransform()->GetLocalPosition();
		Vec3 scale = box->GetTransform()->GetLocalScale();

		float left = (WINDOW.width / 2.0f) + pos.x - scale.x / 2.0f;
		float top = (WINDOW.height / 2.0f) - pos.y - scale.y / 2.0f;

		if (pt.x >= left && pt.x <= left + scale.x &&
			pt.y >= top && pt.y <= top + scale.y)
		{
			_hoveredRoomIndex = i;
			break;
		}
	}

	if (_hoveredRoomIndex != -1)
	{
		// Arrow를 해당 Room Box 오른쪽에 위치
		auto& box = _roomBoxes[_hoveredRoomIndex];
		Vec3 pos = box->GetTransform()->GetLocalPosition();
		Vec3 scale = box->GetTransform()->GetLocalScale();
		_arrowMark->GetTransform()->SetLocalPosition(Vec3(pos.x + scale.x / 2 + 40, pos.y, pos.z));
		_arrowMark->GetMeshRenderer()->SetRender(true);
	}
	else
	{
		_arrowMark->GetMeshRenderer()->SetRender(false);
	}
}

void RoomSelectScene::LoadingSceneRender()
{

}
