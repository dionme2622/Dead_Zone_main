#pragma once
#include "MonoBehaviour.h"
#include <bullet3\btBulletDynamicsCommon.h>

class GameObject;
class ZombieScript : public MonoBehaviour
{
public:
	ZombieScript(vector<shared_ptr<GameObject>>	player);
	virtual ~ZombieScript();

public:
	virtual void FinalUpdate() override;

private:
	vector<shared_ptr<GameObject>>		_player;

	float _speed = 1.0f;
};

