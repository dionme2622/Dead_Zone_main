#pragma once
#include "MonoBehaviour.h"
#include <bullet3\btBulletDynamicsCommon.h>

class CharacterController;
class PlayerScript : public MonoBehaviour
{
public:
	PlayerScript(HWND hwnd, bool isLocal, int playerId, shared_ptr<CharacterController> controller);
	virtual ~PlayerScript();

	virtual void FinalUpdate() override;

	void UpdatePlayerInput();

	void UpdateRotation(float deltaX, float deltaY);

	void UpdateKeyInput();

	void UpdateMouseInput();

	void UpdateCameraPosition();

	void LerpCameraPos();
private:
	HWND _hwnd;
	bool _isLocal;
	int  _playerId;

	shared_ptr<btRigidBody> _body = nullptr;

	float _speed;			// 기존에 있던 이동 속도
	shared_ptr<CharacterController> _controller;

private:
	float sensitivity = 0.001f;

	float _pitch = 0.0f; // 위/아래 각도
	float _yaw = 0.0f; // 좌우 각도
	Vec3 rotation; // 카메라 회전

	bool _mouseMove = true;
	shared_ptr<Transform> _cameraTransform;
	Vec3 _prevPosition{ 0,0,0 };

	bool _isAiming = false;
	Vec3 _targetCameraPos = Vec3(1.2f, 3.03f, -6.65f);
	float _lerpSpeed = 10.0;
};

