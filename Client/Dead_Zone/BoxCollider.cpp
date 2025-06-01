#include "pch.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Resources.h"
#include "Mesh.h"
#include "PhysicsSystem.h"

BoxCollider::BoxCollider(const Vec3& center, const Vec3& extents) : BaseCollider(ColliderType::Box), _center(center), _extents(extents)
{
	// 1) 로컬 박스 모양 정의 (half-extents)
	_shape = std::make_unique<btBoxShape>(
		btVector3(_extents.x, _extents.y, _extents.z));

	// 2) CompoundShape으로 로컬 오프셋(center) 적용
	_compound = std::make_unique<btCompoundShape>();
	btTransform localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(
		btVector3(_center.x, _center.y, _center.z));
	_compound->addChildShape(localTrans, _shape.get());
}		

BoxCollider::~BoxCollider()
{

}

void BoxCollider::FinalUpdate()
{
	auto wm = GetTransform()->GetLocalToWorldMatrix();
	auto trans = Matrix::CreateTranslation(_center.x, _center.y, _center.z);
	GET_SINGLE(DebugRenderer)
		->AddBox(
			{ _center, _extents }, // 로컬 바운딩 정보
			wm,                 // 스케일·회전·위치 모두 포함
			{ 0,1,0,1 }            // 컬러
		);
}

bool BoxCollider::Intersects(const Vec4& rayOrigin, const Vec4& rayDir, float& outDistance) {
	auto world = GET_SINGLE(PhysicsSystem)->GetCollisionWorld();
	btVector3 from(rayOrigin.x, rayOrigin.y, rayOrigin.z);
	btVector3 to(rayOrigin.x + rayDir.x, rayOrigin.y + rayDir.y, rayOrigin.z + rayDir.z);
	btCollisionWorld::ClosestRayResultCallback callback(from, to);
	world->rayTest(from, to, callback);
	if (callback.hasHit()) {
		outDistance = callback.m_closestHitFraction;
		return true;
	}
	return false;
}




shared_ptr<Mesh> BoxCollider::GetColliderMesh()
{
	//Vec3 min = { _boundingBox.Center.x - _boundingBox.Extents.x, _boundingBox.Center.y - _boundingBox.Extents.y, _boundingBox.Center.z - _boundingBox.Extents.z };
	//Vec3 max = { _boundingBox.Center.x + _boundingBox.Extents.x, _boundingBox.Center.y + _boundingBox.Extents.y, _boundingBox.Center.z + _boundingBox.Extents.z };

	//vector<Vertex> vec(24);
	//// 앞면
	//vec[0] = Vertex(Vec3(min.x, min.y, min.z), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	//vec[1] = Vertex(Vec3(min.x, max.y, min.z), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	//vec[2] = Vertex(Vec3(max.x, max.y, min.z), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	//vec[3] = Vertex(Vec3(max.x, min.y, min.z), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	//// 뒷면
	//vec[4] = Vertex(Vec3(min.x, min.y, max.z), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	//vec[5] = Vertex(Vec3(max.x, min.y, max.z), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	//vec[6] = Vertex(Vec3(max.x, max.y, max.z), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	//vec[7] = Vertex(Vec3(min.x, max.y, max.z), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
	//// 윗면
	//vec[8] = Vertex(Vec3(min.x, max.y, min.z), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	//vec[9] = Vertex(Vec3(min.x, max.y, max.z), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	//vec[10] = Vertex(Vec3(max.x, max.y, max.z), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	//vec[11] = Vertex(Vec3(max.x, max.y, min.z), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
	//// 아랫면
	//vec[12] = Vertex(Vec3(min.x, min.y, min.z), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	//vec[13] = Vertex(Vec3(max.x, min.y, min.z), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	//vec[14] = Vertex(Vec3(max.x, min.y, max.z), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	//vec[15] = Vertex(Vec3(min.x, min.y, max.z), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
	//// 왼쪽면
	//vec[16] = Vertex(Vec3(min.x, min.y, max.z), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	//vec[17] = Vertex(Vec3(min.x, max.y, max.z), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	//vec[18] = Vertex(Vec3(min.x, max.y, min.z), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	//vec[19] = Vertex(Vec3(min.x, min.y, min.z), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
	//// 오른쪽면
	//vec[20] = Vertex(Vec3(max.x, min.y, min.z), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	//vec[21] = Vertex(Vec3(max.x, max.y, min.z), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	//vec[22] = Vertex(Vec3(max.x, max.y, max.z), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
	//vec[23] = Vertex(Vec3(max.x, min.y, max.z), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

	//vector<uint32> idx(36);

	//// 앞면
	//idx[0] = 0; idx[1] = 1; idx[2] = 2;
	//idx[3] = 0; idx[4] = 2; idx[5] = 3;
	//// 뒷면
	//idx[6] = 4; idx[7] = 5; idx[8] = 6;
	//idx[9] = 4; idx[10] = 6; idx[11] = 7;
	//// 윗면
	//idx[12] = 8; idx[13] = 9; idx[14] = 10;
	//idx[15] = 8; idx[16] = 10; idx[17] = 11;
	//// 아랫면
	//idx[18] = 12; idx[19] = 13; idx[20] = 14;
	//idx[21] = 12; idx[22] = 14; idx[23] = 15;
	//// 왼쪽면
	//idx[24] = 16; idx[25] = 17; idx[26] = 18;
	//idx[27] = 16; idx[28] = 18; idx[29] = 19;
	//// 오른쪽면
	//idx[30] = 20; idx[31] = 21; idx[32] = 22;
	//idx[33] = 20; idx[34] = 22; idx[35] = 23;

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	//mesh->Init(vec, idx);

	return mesh;
	
}

void BoxCollider::UpdateWorldBounds(const Matrix& worldMat)
{
	Vec3 center = GetCenter();
	Vec3 extents = _extents;

	std::vector<Vec3> corners;
	for (int x = -1; x <= 1; x += 2)
		for (int y = -1; y <= 1; y += 2)
			for (int z = -1; z <= 1; z += 2)
			{
				Vec3 localCorner = center + Vec3(extents.x * x, extents.y * y, extents.z * z);
				Vec3 worldCorner = XMVector3TransformCoord(localCorner, worldMat);
				corners.push_back(worldCorner);
			}

	_worldAABBMin = corners[0];
	_worldAABBMax = corners[0];
	for (const auto& c : corners)
	{
		_worldAABBMin = Vec3::Min(_worldAABBMin, c);
		_worldAABBMax = Vec3::Max(_worldAABBMax, c);
	}

	_worldBoundingCenter = (_worldAABBMin + _worldAABBMax) * 0.5f;
	_worldBoundingRadius = ((_worldAABBMax - _worldAABBMin) * 0.5f).Length();
}
	