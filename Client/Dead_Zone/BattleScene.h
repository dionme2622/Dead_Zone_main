#pragma once
#include "Scene.h"
#include "PlayerStats.h"

#define POOL_SIZE 10


class BattleScene : public Scene
{
public:
	BattleScene();
	virtual ~BattleScene() {}

	virtual void LoadScene();
	virtual void Update();

	void LoadingSceneRender();

	void LoadUI();

	void ShowMuzzleEffect(const Vec3& pos);

public:
	void UpdateSunOrbit();

	void PlayerChaseShadowCamera();

	void DecreaseLightIntensity();

	static bool isPlayerGrounded;
	vector<shared_ptr<GameObject>> GetPlayers() { return _player; };
	vector<shared_ptr<GameObject>>	GetSpotLights() { return _spotLights; }


public:

private:
	shared_ptr<GameObject>							_playerCamera;
	shared_ptr<GameObject>							_uiCamera;

	vector<shared_ptr<GameObject>>					_player;
	vector<shared_ptr<GameObject>>					_heartPoint;
	vector<shared_ptr<GameObject>>					_immunityPoint;

	shared_ptr<GameObject>							_mainLight;
	vector<shared_ptr<GameObject>>					_spotLights;
	shared_ptr<GameObject>							_sunObject;
	HWND _hwnd;


private:
	vector<vector<shared_ptr<GameObject>>> _zombies;


	bool _isAiming = false;
	Vec3 _targetCameraPos = Vec3(1.2f, 3.03f, -6.65f);
	float _lerpSpeed = 10.0;


	int _myID;

	vector<shared_ptr<GameObject>>					 _muzzlePool;



	// 조명 밝기 관련
	float _lightIntensity = 1.0f;
	float _lightDecreaseSpeed = 1.0f / 60.0f; 
	float _lightDimmingElapsed = 0.0f; 
};

