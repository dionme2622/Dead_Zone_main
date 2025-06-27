#include "pch.h"
#include "ResultScene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "Camera.h"

void ResultScene::LoadScene()
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

	shared_ptr<GameObject> result = make_shared<GameObject>();
	result->SetLayerIndex(LayerNameToIndex(L"UI"));
	result->AddComponent(make_shared<Transform>());
	result->GetTransform()->SetLocalPosition(Vec3(0, 0, 1.f));
	result->GetTransform()->SetLocalScale(Vec3(1920, 1080, 1.f));
	result->SetCheckFrustum(false);
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"UI");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"Result", L"..\\Resources\\Texture\\Result.png");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	result->AddComponent(meshRenderer);
	AddGameObject(result);

}

void ResultScene::Update()
{
	Scene::Update();
}


