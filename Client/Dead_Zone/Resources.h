#pragma once

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Resources
{
	DECLARE_SINGLE(Resources);

public:
	void Init();

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool Add(const wstring& key, shared_ptr<T>& object);

	template<typename T>
	shared_ptr<T> Get(const wstring& Key);

	template<typename T>
	OBJECT_TYPE GetObjectType();

	shared_ptr<Mesh> LoadPointMesh();
	shared_ptr<Mesh> LoadRectangleMesh();
	shared_ptr<Mesh> LoadCubeMesh();
	shared_ptr<Mesh> LoadSphereMesh();
	shared_ptr<class MeshData> LoadModelFromBinary(const wstring& path, int type = OBJECT);
	shared_ptr<class AnimatorController> LoadAnimatorPlayerController();			// Animation Controller를 생성한다.
	shared_ptr<class AnimatorController> LoadAnimatorZombieController();			// Animation Controller를 생성한다.

	//void LoadSceneFromBinary(const wstring& path);

	shared_ptr<Texture> CreateTexture(const wstring& name, DXGI_FORMAT format, uint64 width, uint64 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE, Vec4 clearColor = Vec4());

	shared_ptr<Texture> CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource>& tex2D);


	void AddAnimClip(const wstring& key, shared_ptr<AnimClipInfo> clip);
	shared_ptr<AnimClipInfo> GetAnimClip(const wstring& key) const;
private:
	void CreateDefaultShader();
	void CreateDefaultMaterial();

private:
	using KeyObjMap = map<wstring/*key*/, shared_ptr<Object>>;
	array<KeyObjMap, OBJECT_TYPE_COUNT> _resources;

	unordered_map<wstring, shared_ptr<AnimClipInfo>> _animClips;		// 애니메이션 클립 map

	
};

template<typename T>
inline shared_ptr<T> Resources::Load(const wstring& key, const wstring& path)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);			// 만약 key 값이 있다면 value class 객체를 return 한다.
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	shared_ptr<T> object = make_shared<T>();
	object->Load(path);
	keyObjMap[key] = object;

	return object;
}

template<typename T>
bool Resources::Add(const wstring& key, shared_ptr<T>& object)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	//auto findIt = keyObjMap.find(key);
	//if (findIt != keyObjMap.end())			// 만약 key 값이 이미 있다면 Add 하지 않는다.
	//	return false;

	keyObjMap[key] = object;

	return true;
}

template<typename T>
shared_ptr<T> Resources::Get(const wstring& key)
{
	OBJECT_TYPE objectType = GetObjectType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}

template<typename T>
inline OBJECT_TYPE Resources::GetObjectType()
{
	if (is_same_v<T, GameObject>)
		return OBJECT_TYPE::GAMEOBJECT;
	else if (is_same_v<T, Material>)
		return OBJECT_TYPE::MATERIAL;
	else if (is_same_v<T, Mesh>)
		return OBJECT_TYPE::MESH;
	else if (is_same_v<T, Shader>)
		return OBJECT_TYPE::SHADER;
	else if (is_same_v<T, Texture>)
		return OBJECT_TYPE::TEXTURE;
	else if (is_convertible_v<T, Component>)
		return OBJECT_TYPE::COMPONENT;
	else
		return OBJECT_TYPE::NONE;
}

