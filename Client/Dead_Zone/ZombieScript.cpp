#include "pch.h"
#include "ZombieScript.h"
#include "Engine.h"
#include "Timer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator.h"
#include "RigidBody.h"
ZombieScript::ZombieScript(vector<shared_ptr<GameObject>> player)
{
	_player = player;
	_speed = 1.0f;

}

ZombieScript::~ZombieScript()
{
}

void ZombieScript::FinalUpdate()
{
	const float detectionRange = 20.0f;
	const float detectionRangeSq = detectionRange * detectionRange;


	auto  trans = GetTransform();
	Vec3  zPos = trans->GetLocalPosition();

	// --- 1) 가장 가까운 플레이어까지의 거리 제곱 찾기
	float minDistSq = FLT_MAX;
	Vec3  closestPos{};
	for (auto& player : _player)
	{
		Vec3 delta = player->GetTransform()->GetLocalPosition() - zPos;
		float d2 = delta.LengthSquared();
		if (d2 < minDistSq)
		{
			minDistSq = d2;
			closestPos = player->GetTransform()->GetLocalPosition();
		}
	}

	// --- 2) 범위 밖이면 '가만히' 놔둔다
	if (minDistSq > detectionRangeSq)
	{
		// 예: Idle 애니메이션 재생
		_speed = 0.f;
		Vec3 dir = closestPos - zPos;
		dir.Normalize();
		Vec3  move = dir * _speed * DELTA_TIME;
		GetCharacterController()->Move(move);
		GetAnimator()->SetBool("isWalking", false);
	}
	else {
		// --- 3) 범위 내면 추격
		Vec3 dir = closestPos - zPos;
		dir.Normalize();
		_speed = 1.0f;
		// 이	동
		Vec3  move = dir * _speed * DELTA_TIME;
		GetCharacterController()->Move(move);

		// 회전 (Yaw)
		float yawRad = atan2f(dir.x, dir.z);
		Vec3  rot = trans->GetLocalRotation();
		rot.y = yawRad * (180.0f / XM_PI);
		trans->SetLocalRotation(rot);

		// 예: Run 애니메이션 재생
		GetAnimator()->SetBool("isWalking", true);
	}
}

