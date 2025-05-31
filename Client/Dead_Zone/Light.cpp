#include "pch.h"
#include "Light.h"
#include "Transform.h"
#include "Engine.h"
#include "Resources.h"
#include "Camera.h"
#include "Texture.h"
#include "SceneManager.h"
#include "KeyInput.h"

Light::Light() : Component(COMPONENT_TYPE::LIGHT)
{
	_shadowCamera = make_shared<GameObject>();
	_shadowCamera->AddComponent(make_shared<Transform>());
	_shadowCamera->AddComponent(make_shared<Camera>());
	_shadowCamera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
}

Light::~Light()
{
}

void Light::FinalUpdate()
{
	_lightInfo.position = GetTransform()->GetWorldPosition();

	Vec3 lightDir = Vec3(_lightInfo.direction.x, _lightInfo.direction.y, _lightInfo.direction.z);

	_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());

	//_shadowCamera->GetTransform()->SetLocalRotation(Vec3(35.0, 15.01, 0.01));

	SetCameraRotationFromDirection(_shadowCamera->GetTransform(), lightDir);

	//cout << lightDir.x << ", " << lightDir.y << ", " << lightDir.z << endl;

	//Vec3 rot = _shadowCamera->GetTransform()->GetLocalRotation();

	_shadowCamera->FinalUpdate();
}

void Light::Render()
{
	assert(_lightIndex >= 0);
	GetTransform()->PushData();

	if (static_cast<LIGHT_TYPE>(_lightInfo.lightType) == LIGHT_TYPE::DIRECTIONAL_LIGHT)
	{
		shared_ptr<Texture> shadowTex = GET_SINGLE(Resources)->Get<Texture>(L"ShadowTarget");
		_lightMaterial->SetTexture(2, shadowTex);

		Matrix matVP = _shadowCamera->GetCamera()->GetViewMatrix() * _shadowCamera->GetCamera()->GetProjectionMatrix();

		_lightMaterial->SetMatrix(0, matVP);
	}
	else
	{
		float scale = 2 * _lightInfo.range;
		GetTransform()->SetLocalScale(Vec3(scale, scale, scale));
	}

	_lightMaterial->SetInt(0, _lightIndex);
	_lightMaterial->PushGraphicsData();
	_volumeMesh->Render();
}

void Light::RenderShadow()
{
	_shadowCamera->GetCamera()->SortShadowObject();
	_shadowCamera->GetCamera()->Render_Shadow();
}

void Light::SetLightDirection(Vec3 direction)
{
	direction.Normalize();

	_lightInfo.direction = direction;
	GetTransform()->LookAt(direction);

}

void Light::SetLightType(LIGHT_TYPE type)
{
	_lightInfo.lightType = static_cast<int32>(type);

	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"DirLight");

		_shadowCamera->GetCamera()->SetScale(1.f);
		_shadowCamera->GetCamera()->SetNear(50);
		_shadowCamera->GetCamera()->SetFar(500);
		_shadowCamera->GetCamera()->SetWidth(400);
		_shadowCamera->GetCamera()->SetHeight(400);

		break;
	case LIGHT_TYPE::POINT_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		_volumeMesh = GET_SINGLE(Resources)->Get<Mesh>(L"Sphere");
		_lightMaterial = GET_SINGLE(Resources)->Get<Material>(L"PointLight");
		break;
	}
}


void Light::SetCameraRotationFromDirection(shared_ptr<Transform> transform, Vec3 lightDir) {
	Vec3 dir = lightDir;

	// 오일러 각도 계산 (도 단위)
	float yaw = atan2f(dir.x, dir.z) * (180.0f / XM_PI); // Y축 회전
	float pitch = asinf(-dir.y) * (180.0f / XM_PI);      // X축 회전
	float roll = 0.0f;                                    // Z축 회전 (고정)

	// 오일러 각도를 Transform에 적용
	transform->SetLocalRotation(Vec3(pitch, yaw, roll));
}
