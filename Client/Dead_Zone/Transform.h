#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	Transform();
	virtual ~Transform();

	virtual void Update() override;
	virtual void FinalUpdate() override;
	Matrix GetToRootTransform();
	Matrix GetLocalMatrix();
	Matrix GetLocalMatrix2();
	void SetNo(bool no) { _no = no; }
	bool _no = false;
	void PushData();

	void PushShadowData();

public:
	// Parent 기준
	const Vec3& GetLocalPosition() { return _localPosition; }
	const Vec3& GetLocalRotation() { return _localRotation; }
	const Vec3& GetLocalScale() { return _localScale; }

	// TEMP
	float GetBoundingSphereRadius() { return max(max(_localScale.x, _localScale.y), _localScale.z); }

	const Matrix& GetLocalToWorldMatrix() { return _matWorld; }
	Vec3 GetWorldPosition() { return _matWorld.Translation(); }
	Vec3 GetPrevWorldPosition() { return _matPrevWorld.Translation(); }

	Vec3 GetRight() { return _matWorld.Right(); }
	Vec3 GetUp() { return _matWorld.Up(); }
	Vec3 GetLook() { return _matWorld.Backward(); }

	void SetLocalPosition(const Vec3& position) { _localPosition = position; }
	void SetLocalRotation(const Vec3& rotation) { _localRotation = rotation; }
	void SetLocalScale(const Vec3& scale) { _localScale = scale; }
	void SetLocalMatrix(Matrix& matrix) { _matLocal = matrix;}
	void LookAt(const Vec3& dir);
	void LightLookAt(const Vec3& dir);

	static bool CloseEnough(const float& a, const float& b, const float& epsilon = numeric_limits<float>::epsilon());
	static Vec3 DecomposeRotationMatrix(const Matrix& rotation);
	// 라이트 전용 함수
	Vec3 DecomposeRotationMatrixForLight(const Matrix& rotation, const Vec3& lightDir);

	shared_ptr<Transform> Clone();

public:
	void SetParent(shared_ptr<Transform> parent) { _parent = parent; }
	weak_ptr<Transform> GetParent() { return _parent; }

private:
	// Parent 기준
	Vec3 _localPosition = {};
	Vec3 _localRotation = {};
	Vec3 _localScale = { 1.f, 1.f, 1.f };

	Matrix _matLocal = {};
	Matrix _matWorld = {};
	Matrix _matPrevWorld = {};

	weak_ptr<Transform> _parent;




	// 디버그
public:
	bool debug = false;
};

