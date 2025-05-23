#pragma once
#include "Object.h"

enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	PARTICLE_SYSTEM,
	COLLIDER,
	RIGIDBODY,
	CHARACTER_CONTROLLER,
	COLLISION_OBJECT,
	ANIMATOR,
	WEAPONMANAGER,
	WEAPON,
	BULLET,
	// ...
	PLAYERSTATS,
	// ...
	MONO_BEHAVIOUR,			// 반드시 END 전에 위치해야한다.
	END,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1
};

class GameObject;
class Transform;
class MeshRenderer;
class Animator;
class BaseCollider;
class WeaponManager;
class Weapon;
class Bullet;
class PlayerStats;
class RigidBody;
class CharacterController;

class Component : public Object
{
public:
	Component(COMPONENT_TYPE type);
	virtual ~Component();

public:
	virtual void Awake() { }
	virtual void Start() { }
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void FinalUpdate() { }

public:
	COMPONENT_TYPE GetType() { return _type; }
	bool IsValid() { return _gameObject.expired() == false; }

	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();
	shared_ptr<BaseCollider> GetCollider();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<Animator> GetAnimator();
	shared_ptr<WeaponManager> GetWeaponManager();
	shared_ptr<Weapon> GetWeapon();
	shared_ptr<Bullet> GetBullet();
	shared_ptr<PlayerStats> GetPlayerStats();
	shared_ptr<RigidBody> GetRigidBody();
	shared_ptr<CharacterController> GetCharacterController();

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

protected:
	COMPONENT_TYPE _type;
	weak_ptr<GameObject> _gameObject;
};

