#pragma once

class Mesh;
class Material;
class Transform;
class btVector3;

struct MaterialInfo
{
	Vec4			albedo;
	Vec4			emissive;
	Vec4			specular;
	float			glossiness;
	float			smoothness;
	float			metalic;
	float			specularhighlight;
	float			glossyreflection;

	wstring			name;
	wstring			albedoTexName;
	wstring			normalTexName;
	wstring			specularTexName;
};

struct BinaryMeshInfo
{
	wstring								objName;
	wstring								meshName;
	vector<Vertex>						vertices;
	vector<btVector3>					btvertices;
	vector<vector<uint32>>				indices;
	Vec3								AABBCenter;
	Vec3								AABBExtents;
	vector<MaterialInfo>				materials;
	shared_ptr<Transform>				transform;
	bool								hasMesh = false;
	bool								hasAnimation = false;
};

struct BinaryBoneInfo
{
	wstring					boneName;
	Matrix					matOffset;
};

struct BinaryKeyFrameInfo
{
	wstring		boneName;
	Matrix		matTransform;
	double		time;
};

struct BinaryAnimClipInfo
{
	wstring								name;
	double								duration;
	int32								frameCount;
	vector<vector<BinaryKeyFrameInfo>>	keyFrames;
};

class BinaryLoader
{
public:
	BinaryLoader();
	~BinaryLoader();

public:
	void LoadModelFromBinary(const char* path);
	void LoadFrameHierarchyFromFile(shared_ptr<Transform>, FILE*);
	void LoadMeshFromFile(BinaryMeshInfo&, FILE*);
	void LoadMaterialFromFile(BinaryMeshInfo&, FILE*);
	void LoadSkinInfoFromFile(BinaryMeshInfo&, FILE*);
	void LoadAnimationFromFile(vector<BinaryMeshInfo>&, FILE*);

	void CreateTextures();
	void CreateMaterials();
public:
	int32 GetMeshCount() { return static_cast<int32>(_meshes.size()); }
	const BinaryMeshInfo& GetMesh(int32 idx) { return _meshes[idx]; }

	vector<shared_ptr<BinaryBoneInfo>>& GetBones() { return _bones; }
	vector<shared_ptr<BinaryAnimClipInfo>>& GetAnimClip() { return _animClips; }

private:
	vector<BinaryMeshInfo>					_meshes;
	vector<shared_ptr<BinaryBoneInfo>>		_bones;
	vector<shared_ptr<BinaryAnimClipInfo>>	_animClips;


};

