#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{

}

Transform::~Transform()
{

}

void Transform::Update()
{
	/*float radX = _localRotation.x * (XM_PI / 180.0f);
	float radY = _localRotation.y * (XM_PI / 180.0f);
	float radZ = _localRotation.z * (XM_PI / 180.0f);

	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationZ(radZ)
		* Matrix::CreateRotationX(radX)
		* Matrix::CreateRotationY(radY);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matScale * matRotation * matTranslation;

	_matWorld = _matLocal;

	if (!_no)
	{
		shared_ptr<Transform> parent = GetParent().lock();
		if (parent != nullptr)
		{
			_matWorld *= parent->GetLocalToWorldMatrix();
		}
	}*/
}

void Transform::FinalUpdate()
{
	float radX = _localRotation.x * (XM_PI / 180.0f);
	float radY = _localRotation.y * (XM_PI / 180.0f);
	float radZ = _localRotation.z * (XM_PI / 180.0f);

	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationZ(radZ)
					   * Matrix::CreateRotationX(radX)
					   * Matrix::CreateRotationY(radY);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matScale * matRotation * matTranslation;

	_matWorld = _matLocal;

	if (!_no)
	{
		shared_ptr<Transform> parent = GetParent().lock();
		if (parent != nullptr)
		{
			_matWorld *= parent->GetLocalToWorldMatrix();
		}
	}
}

Matrix Transform::GetToRootTransform()
{
	_matWorld = _matLocal;

	shared_ptr<Transform> parent = GetParent().lock();
	if (parent != nullptr)
	{
		_matWorld *= parent->GetLocalToWorldMatrix();
	}

	return _matWorld;
}

Matrix Transform::GetLocalMatrix()
{
	float radX = _localRotation.x * (XM_PI / 180.0f);
	float radY = _localRotation.y * (XM_PI / 180.0f);
	float radZ = _localRotation.z * (XM_PI / 180.0f);

	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationZ(radZ)
		* Matrix::CreateRotationX(radX)
		* Matrix::CreateRotationY(radY);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	Matrix matLocal = matScale * matRotation * matTranslation;


	Matrix matWorld = matLocal;

	shared_ptr<Transform> parent = GetParent().lock();
	if (parent != nullptr)
	{
		matWorld *= parent->GetLocalMatrix();
	}

	return matWorld;
}

Matrix Transform::GetLocalMatrix2()
{
	float radX = _localRotation.x * (XM_PI / 180.0f);
	float radY = _localRotation.y * (XM_PI / 180.0f);
	float radZ = _localRotation.z * (XM_PI / 180.0f);

	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationZ(radZ)
		* Matrix::CreateRotationX(radX)
		* Matrix::CreateRotationY(radY);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	Matrix matLocal = matScale * matRotation * matTranslation;


	Matrix matWorld = matLocal;



	return matWorld;
}

void Transform::PushData()
{
	TransformParams transformParams = {};
	transformParams.matWorld = _matWorld;
	transformParams.matView = Camera::S_MatView;
	transformParams.matProjection = Camera::S_MatProjection;
	transformParams.matWV = _matWorld * Camera::S_MatView;
	transformParams.matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
	transformParams.matViewInv = Camera::S_MatView.Invert();

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParams, sizeof(transformParams));
}

void Transform::PushShadowData()
{
	TransformParams transformParams = {};

	Matrix shadowScale = Matrix::CreateScale(1.0005f, 1.0005f, 1.0005f); 
	Matrix shadowWorld = shadowScale * _matWorld;

	transformParams.matWorld = shadowWorld;
	transformParams.matView = Camera::S_MatView;
	transformParams.matProjection = Camera::S_MatProjection;
	transformParams.matWV = shadowWorld * Camera::S_MatView;
	transformParams.matWVP = shadowWorld * Camera::S_MatView * Camera::S_MatProjection;
	transformParams.matViewInv = Camera::S_MatView.Invert();

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParams, sizeof(transformParams));
}

void Transform::LookAt(const Vec3& dir)
{
	Vec3 front = dir;
	front.Normalize();

	Vec3 right = Vec3::Up.Cross(dir);
	if (right == Vec3::Zero)
		right = Vec3::Forward.Cross(dir);

	right.Normalize();

	Vec3 up = front.Cross(right);
	up.Normalize();

	Matrix matrix = XMMatrixIdentity();
	matrix.Right(right);
	matrix.Up(up);
	matrix.Backward(front);

	_localRotation = DecomposeRotationMatrix(matrix);
}

void Transform::LightLookAt(const Vec3& dir)
{
	Vec3 front = dir;
	front.Normalize();

	if (front.LengthSquared() < 0.0001f) {
		std::cout << "Error: Invalid LookAt direction: " << dir.x << ", " << dir.y << ", " << dir.z << std::endl;
		return;
	}

	Vec3 right = Vec3::Up.Cross(front);
	if (right.LengthSquared() < 0.001f) {
		right = Vec3::Forward.Cross(front);
		if (right.LengthSquared() < 0.001f) {
			right = Vec3::Right.Cross(front);
		}
	}
	right.Normalize();

	Vec3 up = front.Cross(right);
	up.Normalize();

	Matrix matrix = XMMatrixIdentity();
	matrix.Right(right);
	matrix.Up(up);
	matrix.Backward(front);

	_localRotation = DecomposeRotationMatrixForLight(matrix, front);
}

bool Transform::CloseEnough(const float& a, const float& b, const float& epsilon)
{
	return (epsilon > abs(a - b));
}

Vec3 Transform::DecomposeRotationMatrix(const Matrix& rotation)
{
	Vec4 v[4];
	XMStoreFloat4(&v[0], rotation.Right());
	XMStoreFloat4(&v[1], rotation.Up());
	XMStoreFloat4(&v[2], rotation.Backward());
	XMStoreFloat4(&v[3], rotation.Translation());

	Vec3 ret;
	if (CloseEnough(v[0].z, -1.0f))
	{
		float x = 0;
		float y = XM_PI / 2;
		float z = x + atan2(v[1].x, v[2].x);
		ret = Vec3{ x, y, z };
	}
	else if (CloseEnough(v[0].z, 1.0f))
	{
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2(-v[1].x, -v[2].x);
		ret = Vec3{ x, y, z };
	}
	else
	{
		float y1 = -asin(v[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(v[1].z / cos(y1), v[2].z / cos(y1));
		float x2 = atan2f(v[1].z / cos(y2), v[2].z / cos(y2));

		float z1 = atan2f(v[0].y / cos(y1), v[0].x / cos(y1));
		float z2 = atan2f(v[0].y / cos(y2), v[0].x / cos(y2));

		if ((abs(x1) + abs(y1) + abs(z1)) <= (abs(x2) + abs(y2) + abs(z2)))
		{
			ret = Vec3{ x1, y1, z1 };
		}
		else
		{
			ret = Vec3{ x2, y2, z2 };
		}
	}

	return ret;
}

Vec3 Transform::DecomposeRotationMatrixForLight(const Matrix& rotation, const Vec3& lightDir)
{
	// 1. 방향 → 쿼터니언
	XMVECTOR f = XMVector3Normalize(XMLoadFloat3(&lightDir));
	XMVECTOR u = XMVectorSet(0, 1, 0, 0);
	XMVECTOR r = XMVector3Normalize(XMVector3Cross(u, f));
	u = XMVector3Cross(f, r);

	XMMATRIX m = XMMatrixIdentity();
	m.r[0] = r;   // Right  (X)
	m.r[1] = u;   // Up     (Y)
	m.r[2] = f;   // Forward(Z)

	XMVECTOR qv = XMQuaternionNormalize(XMQuaternionRotationMatrix(m));

	// 2. 쿼터니언 → 오일러(Z-Y-X: Yaw-Pitch-Roll)
	XMFLOAT4 q;  XMStoreFloat4(&q, qv);

	Vec3 e{};
	float sinp = 2.f * (q.w * q.y - q.z * q.x);     // Pitch test
	const float k = 0.9999f;

	if (fabsf(sinp) >= k) {                         // ±90° 근처(gimbal-lock)
		e.y = copysignf(XM_PIDIV2, sinp);           // Pitch(Y)
		e.z = 0.f;                                  // Yaw(Z)
		e.x = atan2f(2.f * (q.x * q.y + q.w * q.z),
			1.f - 2.f * (q.y * q.y + q.z * q.z)); // Roll(X)
	}
	else {
		e.y = asinf(sinp);                          // Pitch(Y)
		e.z = atan2f(2.f * (q.w * q.x + q.y * q.z),
			1.f - 2.f * (q.x * q.x + q.y * q.y)); // Yaw(Z)
		e.x = atan2f(2.f * (q.w * q.z + q.x * q.y),
			1.f - 2.f * (q.y * q.y + q.z * q.z)); // Roll(X)
	}

	// 라디안 → 도
	e.x = XMConvertToDegrees(e.x);
	e.y = XMConvertToDegrees(e.y);
	e.z = XMConvertToDegrees(e.z);
	return e;
}


shared_ptr<Transform> Transform::Clone()
{
	shared_ptr<Transform> transform = make_shared<Transform>();

	transform->SetLocalPosition(GetLocalPosition());
	transform->SetLocalRotation(GetLocalRotation());
	transform->SetLocalScale(GetLocalScale());
	//transform->SetParent(GetParent());

	return transform;
}