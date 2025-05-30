#include "pch.h"
#include "MeshData.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "BinaryLoader.h"
#include "Animator.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "MeshCollider.h"
#include "AnimatorController.h"

MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA)
{
}

MeshData::~MeshData()
{
}

shared_ptr<MeshData> MeshData::LoadModelFromBinary(const char* path, int type)
{
	BinaryLoader loader;
	loader.LoadModelFromBinary(path);

	shared_ptr<MeshData> meshData = make_shared<MeshData>();

	for (int32 i = 0; i < loader.GetMeshCount(); i++)
	{
		MeshRenderInfo info = {};

		// Mesh
		if (!loader.GetMesh(i).vertices.empty())
		{
			shared_ptr<Mesh> mesh = Mesh::CreateFromBinary(&loader.GetMesh(i), loader, type);
			mesh->SetName(loader.GetMesh(i).meshName);
			GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);

			shared_ptr<MeshCollider> meshCollider = make_shared<MeshCollider>(loader.GetMesh(i).btvertices, loader.GetMesh(i).indices[0], false);
			//info.mesh = GET_SINGLE(Resources)->Get<Mesh>(mesh->GetName());
			info.mesh = mesh;
			info.meshCollider = meshCollider;
		}

		// Transform
		shared_ptr<Transform> transform = loader.GetMesh(i).transform;

		// Material
		vector<shared_ptr<Material>> materials;
		for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++)
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(loader.GetMesh(i).meshName);

			materials.push_back(material);
		}

		// Collider 
		Vec3 center = loader.GetMesh(i).AABBCenter;
		Vec3 extents = loader.GetMesh(i).AABBExtents;
		shared_ptr<BoxCollider> boxCollider = make_shared<BoxCollider>(center, extents);


		info.objName = loader.GetMesh(i).objName;
		info.materials = materials;
		info.transform = transform;
		info.boxCollider = boxCollider;


		////////////////////////////////////////
		meshData->_meshRenders.push_back(info);
	}

	return meshData;
}


vector<shared_ptr<GameObject>> MeshData::Instantiate(int type, int collidertype)
{
	vector<shared_ptr<GameObject>> v;

	for (MeshRenderInfo& info : _meshRenders)
	{
		shared_ptr<GameObject> gameObject = make_shared<GameObject>();
		gameObject->SetName(info.objName);									// Object 이름 설정
		gameObject->AddComponent(info.transform);
		if (info.mesh != nullptr)
		{
			gameObject->AddComponent(make_shared<MeshRenderer>());
			gameObject->GetMeshRenderer()->SetMesh(info.mesh);
			for (uint32 i = 0; i < info.materials.size(); i++)
				gameObject->GetMeshRenderer()->SetMaterial(info.materials[i], i);

			
			// TODO : AABB 바운딩 박스 데이터 넘겨야 함
			switch (collidertype)
			{
			case NONE:
				break;
			case BOX:
				if (info.boxCollider != nullptr)
				{
					shared_ptr<BoxCollider> collider = info.boxCollider;
					gameObject->AddComponent(collider);
					gameObject->AddComponent(make_shared<RigidBody>(gameObject, 0.0f, dynamic_pointer_cast<BoxCollider>(gameObject->GetCollider()), gameObject->GetTransform()->GetLocalMatrix(), false));
					gameObject->GetRigidBody()->OnEnable();
				}
				break;
			case MESH:
				if (info.meshCollider != nullptr)
				{
					shared_ptr<MeshCollider> collider = info.meshCollider;
					gameObject->AddComponent(collider);
					gameObject->AddComponent(make_shared<RigidBody>(gameObject, 0.0f, dynamic_pointer_cast<MeshCollider>(gameObject->GetCollider()), gameObject->GetTransform()->GetLocalMatrix() , false));
					gameObject->GetRigidBody()->OnEnable();

				}
				break;
			}
			
			if (info.mesh->hasAnimation())				// Mesh가 애니메이션을 가지고 있다면?
			{
				shared_ptr<Animator> animator = nullptr;
				switch (type)
				{
				case PLAYER:
					animator = make_shared<Animator>(GET_SINGLE(Resources)->LoadAnimatorPlayerController());
					animator->SetBones(info.mesh->GetBones());
					gameObject->AddComponent(animator);
					break;
				case ZOMBIE:
					animator = make_shared<Animator>(GET_SINGLE(Resources)->LoadAnimatorZombieController());
					animator->SetBones(info.mesh->GetBones());
					gameObject->AddComponent(animator);
					break;
				default:
					break;
				}
				
			}
		}
		v.push_back(gameObject);
	}

	return v;
}

