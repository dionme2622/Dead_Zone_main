#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Resources.h"
#include "Animator.h"
#include "InstancingBuffer.h"
#include "Shader.h"
#include "StructuredBuffer.h"
#include "WeaponManager.h"
#include "Weapon.h"

MeshRenderer::MeshRenderer() : Component(COMPONENT_TYPE::MESH_RENDERER)
{
}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::SetMaterial(shared_ptr<Material> material, uint32 idx)
{
	if (_materials.size() <= static_cast<size_t>(idx))
		_materials.resize(static_cast<size_t>(idx + 1));

	_materials[idx] = material;
}

void MeshRenderer::Render()
{
	for (uint32 i = 0; i < _materials.size(); i++)
	{
		shared_ptr<Material>& material = _materials[i];
		material->SetInt(0, 0);

		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		switch (material->GetShader()->GetShaderType())
		{
		case SHADER_TYPE::DEFERRED:
			if (!_WIRE_FRAME_MODE)
				material->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"Deferred"));
			else
				material->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"Deferred_Wire"));
			break;
		// TODO : 다른 셰이더 타입
		default:
			break;
		}

		GetTransform()->PushData();

		
		if (GetAnimator())
		{
			GetAnimator()->PushData();
			/*auto buffer = GetAnimator()->GetBoneFinalMatrix();
			buffer->PushGraphicsData(SRV_REGISTER::t7);*/
			material->SetInt(1, 1);
		}


		if (GetWeaponManager())
		{
			GetWeaponManager()->PushData();
		}
		if (GetWeapon())
		{
			GetWeapon()->PushData();
		}

		material->PushGraphicsData();
		_mesh->Render(1, i, _isRender);
	}
}

void MeshRenderer::Render(shared_ptr<InstancingBuffer>& buffer)			// Instancing Version
{
	for (uint32 i = 0; i < _materials.size(); i++)
	{
		shared_ptr<Material>& material = _materials[i];
		material->SetInt(0, 1);

		if (material == nullptr || material->GetShader() == nullptr)
			continue;

		switch (material->GetShader()->GetShaderType())
		{
		case SHADER_TYPE::DEFERRED:
			if (!_WIRE_FRAME_MODE)
				material->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"Deferred"));
			else
				material->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"Deferred_Wire"));
			break;
		// TODO : 다른 셰이더 타입
		default:
			break;
		}

		buffer->PushData();

		if (GetAnimator())
		{
			GetAnimator()->PushData();
		/*	auto buffer = GetAnimator()->GetBoneFinalMatrix();
			buffer->PushGraphicsData(SRV_REGISTER::t7);*/
			material->SetInt(1, 1);
		}

		if (GetWeaponManager())
		{
			GetWeaponManager()->PushData();

		}
		if (GetWeapon())
		{
			GetWeapon()->PushData();
		}

		material->PushGraphicsData();
		_mesh->Render(buffer, i, _isRender);
	}
}

void MeshRenderer::RenderShadow()
{
	GetTransform()->PushData();
	shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Shadow");
	if (GetAnimator())		// Animator을 갖고 있는 객체라면
	{
		GetAnimator()->PushData();
		material->SetInt(3, 1);
		/*auto buffer = GetAnimator()->GetBoneFinalMatrix();
		buffer->PushGraphicsData(SRV_REGISTER::t7);*/
	}
	else
	{
		material->SetInt(3, 0);

	}
	material->PushGraphicsData();
	_mesh->Render();
}

uint64 MeshRenderer::GetInstanceID()
{
	if (_mesh == nullptr || _materials.empty())
		return 0;

	//uint64 id = (_mesh->GetID() << 32) | _material->GetID();
	InstanceID instanceID{ _mesh->GetID(), _materials[0]->GetID() };
	return instanceID.id;
}
