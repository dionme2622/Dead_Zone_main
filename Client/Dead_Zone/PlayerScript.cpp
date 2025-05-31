#include "pch.h"
#include "PlayerScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "KeyInput.h"
#include "MouseInput.h"
#include "Timer.h"
#include "Engine.h"
#include "BoxCollider.h"
#include "SceneManager.h"
#include "BattleScene.h"
#include "WeaponManager.h"
#include "Weapon.h"
#include "Animator.h"
#include "RigidBody.h"
#include "PhysicsSystem.h"
PlayerScript::PlayerScript(HWND hwnd, bool isLocal, int playerId, shared_ptr<CharacterController> controller)
{
	_hwnd = hwnd;
	_isLocal = isLocal;
	_playerId = playerId;
	_controller = controller;
	// Player에 대한 정보 초기화 단계

	_speed = 0.0f;
}

PlayerScript::~PlayerScript()
{
}

void PlayerScript::FinalUpdate()
{
	if (_isLocal)
	{
		// 1) 로컬 입력 읽어서 내 캐릭터 움직임 계산 → 클라이언트 예측
		UpdatePlayerInput();
	}
	else
	{
		// 2) 원격 플레이어: 네트워크 상태를 받아서 Transform에 적용
		// TODO : 여기서 다른 플레이어 객체들의 pos 값을 받아와서 GetTransform()->SetLocalPosition(pos); 을 하면 된다.
		// EX) 서버로 부터 자기 ID에 맞는 객체의 pos 값을 받아와서 GetTransform()->SetLocalPosition(pos); 을 하면 된다.
		//GetTransform()->SetLocalPosition(Vec3(0.f, 10 * DELTA_TIME, 0.f));		// 임시

		
		/*auto state = NetworkManager::Get()->GetPlayerState(_playerId);
		ApplyNetworkState(state.position, state.rotationEuler, state.equippedWeapon);*/
	}
}



void PlayerScript::UpdatePlayerInput()
{
	UpdateKeyInput();
	
	UpdateMouseInput();
}

void PlayerScript::UpdateKeyInput()
{
	_speed = 0.f;
	// 1) 이전 위치 저장
	Vec3 currentPos = GetTransform()->GetLocalPosition();

	// 2) WASD 입력에 따른 방향(dir) 계산
	Vec3 dir(0, 0, 0);
	if (INPUT->GetButton(KEY_TYPE::W)) dir += GetTransform()->GetLook(), _speed = 5.0f;
	if (INPUT->GetButton(KEY_TYPE::S)) dir -= GetTransform()->GetLook(), _speed = 5.0f;
	if (INPUT->GetButton(KEY_TYPE::A)) dir -= GetTransform()->GetRight(), _speed = 5.0f;
	if (INPUT->GetButton(KEY_TYPE::D)) dir += GetTransform()->GetRight(), _speed = 5.0f;
	if (INPUT->GetButton(KEY_TYPE::SHIFT) && _speed > 1.0f) _speed = 10.f;
	if (dir.LengthSquared() > 0.0f)
		dir.Normalize();

	const float fixedStep = 1.0f / 60.0f;
	_controller->Move(dir * _speed * fixedStep);

	// 3) 땅에 붙어 있을 때만 점프
	if (INPUT->GetButtonDown(KEY_TYPE::SPACE) && _controller->IsOnGround())
	{
		_controller->Jump();
	}
	
	if (INPUT->GetButton(KEY_TYPE::KEY_1))
	{
		GetWeaponManager()->EquipWeapon(0);
	}
	
	if (INPUT->GetButton(KEY_TYPE::KEY_2))
	{
		GetWeaponManager()->EquipWeapon(1);
	}
	
	if (INPUT->GetButton(KEY_TYPE::KEY_3))
	{	
		GetWeaponManager()->EquipWeapon(2);
	}

	if (INPUT->GetButton(KEY_TYPE::LEFTCLICK))
	{
		// Shoot 애니메이션 트리거
		GetAnimator()->SetTrigger("Shoot");

		GetWeaponManager()->GetCurrentWeapon()[0]->GetWeapon()->SetBulletPosition();
		GetWeaponManager()->GetCurrentWeapon()[0]->GetWeapon()->SetBulletDirection();
		GetWeaponManager()->GetCurrentWeapon()[0]->GetWeapon()->Attack();
	}
	Vec3 delta = currentPos - _prevPosition;
	delta.y = 0;
	float currentSpeed = delta.Length() / DELTA_TIME;

	// 5) Animator 에 전달
	GetAnimator()->SetFloat("Speed", _speed);
	GetAnimator()->SetBool("isJumping", !_controller->IsOnGround());
	//printf("점프?: %d\n", !_controller->IsOnGround());
	//printf("속도: %f\n", currentSpeed);
	/*printf("이전: %f %f %f\n", _prevPosition.x, _prevPosition.y, _prevPosition.z);
	printf("이후: %f %f %f\n", currentPos.x, currentPos.y, currentPos.z);*/

	_prevPosition = currentPos;
}

void PlayerScript::UpdateMouseInput()
{
	if (INPUT->GetButtonDown(KEY_TYPE::Q)) {
		_mouseMove = !_mouseMove;
		ShowCursor(_mouseMove);
	}

	POINT mousePos;
	SetCapture(_hwnd);
	GetCursorPos(&mousePos);
	ScreenToClient(_hwnd, &mousePos);

	POINT center = { WINDOW.width / 2, WINDOW.height / 2 };
	float deltaX = (float)(mousePos.x - center.x);
	float deltaY = (float)(mousePos.y - center.y);

	if (_mouseMove == false)
	{
		if (deltaX != 0 || deltaY != 0)
		{
			UpdateRotation(deltaX, deltaY);
		}

		// 마우스 위치를 중앙으로 고정
		POINT screenCenter = { center.x, center.y };
		ClientToScreen(_hwnd, &screenCenter);
		SetCursorPos(screenCenter.x, screenCenter.y);
	}
}

void PlayerScript::UpdateRotation(float deltaX, float deltaY)
{
	// X축 회전 (Pitch, 위아래) - 카메라에만 적용
	_pitch += deltaY * sensitivity;
	_pitch = max(-90 * XM_PI / 180, min(90 * XM_PI / 180, _pitch));

	// Y축 회전 (Yaw, 좌우) - 캐릭터와 카메라 모두에 적용
	_yaw += deltaX * sensitivity;

	rotation.x = _pitch * 50;
	rotation.y = _yaw * 50;
	rotation.z = 0.0;

	GetTransform()->SetLocalRotation(rotation);
}
