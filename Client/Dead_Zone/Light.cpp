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

	/*if (INPUT->GetButton(KEY_TYPE::UP)) {
		a += 5;
	}
	if (INPUT->GetButton(KEY_TYPE::DOWN)) {
		a -= 5;
	}*/

	_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());

	_shadowCamera->GetTransform()->SetLocalRotation(Vec3(35.0, 15.01, 0.01));

	Vec3 rot = _shadowCamera->GetTransform()->GetLocalRotation();

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
		_shadowCamera->GetCamera()->SetWidth(300);
		_shadowCamera->GetCamera()->SetHeight(300);

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



