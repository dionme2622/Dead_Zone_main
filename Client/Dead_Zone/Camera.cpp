#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include "InstancingManager.h"
#include "BaseCollider.h"
#include "BoxCollider.h"
#include "DebugRenderer.h"
#include "Animator.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Matrix Camera::S_MainMatView;
Matrix Camera::S_MainMatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
	_width = static_cast<float>(GEngine->GetWindow().width);
	_height = static_cast<float>(GEngine->GetWindow().height);
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	if (_type == PROJECTION_TYPE::PERSPECTIVE)
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
	else 
		_matProjection = ::XMMatrixOrthographicLH(_width * _scale, _height * _scale, _near, _far);

	_frustum.FinalUpdate();



}


void Camera::SortGameObject()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecForward.clear();
	_vecDeferred.clear();
	_vecParticle.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr) {
			continue;
		}

		if (IsCulled(gameObject->GetLayerIndex())) {
			continue;
		}

		if (gameObject->GetCheckFrustum())
		{
			Vec3 scale = gameObject->GetTransform()->GetLocalScale();
			if (auto boxCollider = gameObject->GetBoxCollier())
			{
				/*cout << boxCollider->GetCenter().x << boxCollider->GetCenter().y << boxCollider->GetCenter().z << endl;
				cout << gameObject->GetTransform()->GetWorldPosition().x << gameObject->GetTransform()->GetWorldPosition().y << gameObject->GetTransform()->GetWorldPosition().z << endl;
				cout << endl;*/


				//float scaledExtentX = boxCollider->_extents.x;
				//float scaledExtentY = boxCollider->_extents.y;
				//float scaledExtentZ = boxCollider->_extents.z;

				//float maxExtent = max(max(scaledExtentX, scaledExtentY), scaledExtentZ);

				//// 제곱해서 거리 계산
				//float radius = sqrt(pow(maxExtent, 2) * 2); // 여유를 두기 위해 20을 더함);
				//if (_frustum.ContainsSphere(
				//	boxCollider->GetCenter(),
				//	radius/* + 15*/) == false)
				//{
				//	continue;
				//}


				/*Vec3 center = boxCollider->GetCenter();
				Vec3 extents = boxCollider->_extents;
				Matrix worldMat = gameObject->GetTransform()->GetLocalToWorldMatrix();

				std::vector<Vec3> corners;
				for (int x = -1; x <= 1; x += 2)
					for (int y = -1; y <= 1; y += 2)
						for (int z = -1; z <= 1; z += 2)
						{
							Vec3 localCorner = center + Vec3(extents.x * x, extents.y * y, extents.z * z);
							Vec3 worldCorner = XMVector3TransformCoord(localCorner, worldMat);
							corners.push_back(worldCorner);
						}

				Vec3 min = corners[0], max = corners[0];
				for (const auto& c : corners)
				{
					min = Vec3::Min(min, c);
					max = Vec3::Max(max, c);
				}

				Vec3 aabbCenter = (min + max) * 0.5f;
				float aabbRadius = ((max - min) * 0.5f).Length();

				if (_frustum.ContainsSphere(aabbCenter, aabbRadius) == false)
					continue;*/

				if (_frustum.ContainsSphere(boxCollider->_worldBoundingCenter, boxCollider->_worldBoundingRadius) == false)
					continue;
			}

		}

		if (gameObject->GetMeshRenderer())
		{
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::DEFERRED:
				_vecDeferred.emplace_back(gameObject);
				break;
			case SHADER_TYPE::FORWARD:
				_vecForward.emplace_back(gameObject);
				break;
			}
		}
		else
		{
			_vecParticle.emplace_back(gameObject);
		}
	}
}

void Camera::SortShadowObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	S_MatView = _matView;
	S_MatProjection = _matProjection;

	_vecShadow.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		if (gameObject->IsStatic())
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (auto boxCollider = gameObject->GetBoxCollier())
			{
				if (_frustum.ContainsSphere(boxCollider->_worldBoundingCenter, boxCollider->_worldBoundingRadius) == false)
					continue;
			}
		}
		_vecShadow.emplace_back(gameObject);
	}
}

void Camera::Render_Deferred()
{
	if (GetProjectionType() == PROJECTION_TYPE::PERSPECTIVE) {
		S_MainMatView = _matView;
		S_MainMatProjection = _matProjection;
	}

	S_MatView = _matView;
	S_MatProjection = _matProjection;



#ifdef _INSTANCING_MODE
	GET_SINGLE(InstancingManager)->Render(_vecDeferred);

#else
	for (auto& gameObject : _vecDeferred)
	{
		gameObject->GetMeshRenderer()->Render();
	}
#endif

}

void Camera::Render_Forward()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

#ifdef _INSTANCING_MODE
	GET_SINGLE(InstancingManager)->Render(_vecForward);

#else
	for (auto& gameObject : _vecForward)
	{
		gameObject->GetMeshRenderer()->Render();
	}

#endif
	/*for (auto& gameObject : _vecParticle)
	{
		gameObject->GetParticleSystem()->Render();
	}*/

	if (_DEBUG_COLLIDER)
	{
		GET_SINGLE(DebugRenderer)->Flush();
		GET_SINGLE(DebugRenderer)->Clear();
	}
}


void Camera::Render_Shadow()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	if (GetProjectionType() == PROJECTION_TYPE::PERSPECTIVE) {
		S_MainMatView = _matView;
		S_MainMatProjection = _matProjection;
	}


	for (auto& gameObject : _vecShadow)
	{
		/*if(auto animator = gameObject->GetAnimator())
			animator->PushData();*/

		gameObject->GetMeshRenderer()->RenderShadow();
	}
}