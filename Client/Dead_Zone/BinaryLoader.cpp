#include "pch.h"
#include "BinaryLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Resources.h"
#include <bullet3/btBulletDynamicsCommon.h>
#include "PhysicsSystem.h"


BinaryLoader::BinaryLoader()
{
}

BinaryLoader::~BinaryLoader()
{
}

void BinaryLoader::LoadModelFromBinary(const char* path)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, path, "rb");
	::rewind(pInFile);
	char pstrToken[64] = { '\0' };

	shared_ptr<Transform> parentTransform = make_shared<Transform>();


	for (; ; )
	{
		if (::ReadStringFromFile(pInFile, pstrToken))
		{
			if (!strcmp(pstrToken, "<Hierarchy>:"))
			{
				LoadFrameHierarchyFromFile(parentTransform, pInFile);
				::ReadStringFromFile(pInFile, pstrToken); //"</Hierarchy>"
			}
			else if (!strcmp(pstrToken, "<Animation>:"))
			{
				LoadAnimationFromFile(_meshes, pInFile);
			}
			else if (!strcmp(pstrToken, "</Animation>:"))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	::fclose(pInFile);

	CreateTextures();
	CreateMaterials();
}

void BinaryLoader::LoadFrameHierarchyFromFile(shared_ptr<Transform> transform, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	_meshes.push_back(BinaryMeshInfo());
	BinaryMeshInfo& meshInfo = _meshes.back();
	// 임시
	char pstrFrameName[64] = {};
	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ::ReadIntegerFromFile(pInFile);
			nTextures = ::ReadIntegerFromFile(pInFile);
			::ReadStringFromFile(pInFile, pstrToken);

			string name(pstrToken);
			meshInfo.objName = s2ws(name);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion

			// Local Position 값 넣기
			transform->SetLocalPosition(Vec3(xmf3Position));
			transform->SetLocalRotation(Vec3(xmf3Rotation));
			transform->SetLocalScale(Vec3(xmf3Scale));

			meshInfo.transform = transform;
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			Matrix matrix;
			nReads = (UINT)::fread(&matrix, sizeof(float), 16, pInFile);
			//meshInfo.matrix = matrix;
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			LoadMeshFromFile(meshInfo, pInFile);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			LoadSkinInfoFromFile(meshInfo, pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) LoadMeshFromFile(meshInfo, pInFile);

		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			LoadMaterialFromFile(meshInfo, pInFile);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					// TODO : 계층구조를 사용하여 자식의 수 만큼 파일을 재귀함수로 읽는다
					shared_ptr<Transform> childTransform = make_shared<Transform>();
					childTransform->SetParent(transform);
					LoadFrameHierarchyFromFile(childTransform, pInFile);
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
}

void BinaryLoader::LoadMeshFromFile(BinaryMeshInfo& meshes, FILE* pInFile)
{
	//// TODO : 파일에서 Mesh 데이터를 읽어온다.
	char pstrToken[64] = { '\0' };
	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	int32 vertexCount = 0;
	UINT nReads = (UINT)::fread(&vertexCount, sizeof(int), 1, pInFile);

	meshes.vertices.resize(vertexCount);
	meshes.btvertices.resize(vertexCount);
	::ReadStringFromFile(pInFile, pstrToken);			// Mesh의 이름 저장
	string name(pstrToken);
	meshes.meshName = s2ws(pstrToken);

	meshes.hasMesh = true;
	// 임시
	XMFLOAT3 temp = {};
	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&meshes.AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&meshes.AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				XMFLOAT3* m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);

				for (UINT i = 0; i < nPositions; ++i)
				{
					meshes.vertices[i].pos.x = static_cast<float>(m_pxmf3Positions[i].x);
					meshes.vertices[i].pos.y = static_cast<float>(m_pxmf3Positions[i].y);
					meshes.vertices[i].pos.z = static_cast<float>(m_pxmf3Positions[i].z);

					meshes.btvertices[i] = ToBt(meshes.vertices[i].pos);
				}

				delete[] m_pxmf3Positions;
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				XMFLOAT4* m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);

				delete[] m_pxmf4Colors;
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				XMFLOAT2* m_pxmf2TextureCoords0 = new XMFLOAT2[nPositions];
				nReads = (UINT)::fread(m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				for (UINT i = 0; i < nTextureCoords; ++i)
				{
					meshes.vertices[i].uv.x = static_cast<float>(m_pxmf2TextureCoords0[i].x);
					meshes.vertices[i].uv.y = static_cast<float>(m_pxmf2TextureCoords0[i].y);
				}
				delete[] m_pxmf2TextureCoords0;
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				XMFLOAT2* m_pxmf2TextureCoords1 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords1, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				delete[] m_pxmf2TextureCoords1;
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				XMFLOAT3* m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);

				for (UINT i = 0; i < nNormals; ++i)
				{
					meshes.vertices[i].normal.x = static_cast<float>(m_pxmf3Normals[i].x);
					meshes.vertices[i].normal.y = static_cast<float>(m_pxmf3Normals[i].y);
					meshes.vertices[i].normal.z = static_cast<float>(m_pxmf3Normals[i].z);

				}
				delete[] m_pxmf3Normals;
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				XMFLOAT3* m_pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile);

				for (UINT i = 0; i < nTangents; ++i)
				{
					meshes.vertices[i].tangent.x = static_cast<float>(m_pxmf3Tangents[i].x);
					meshes.vertices[i].tangent.y = static_cast<float>(m_pxmf3Tangents[i].y);
					meshes.vertices[i].tangent.z = static_cast<float>(m_pxmf3Tangents[i].z);

				}
				delete[] m_pxmf3Tangents;
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				XMFLOAT3* m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);

				for (UINT i = 0; i < nBiTangents; ++i)
				{
					meshes.vertices[i].bitangent.x = static_cast<float>(m_pxmf3BiTangents[i].x);
					meshes.vertices[i].bitangent.y = static_cast<float>(m_pxmf3BiTangents[i].y);
					meshes.vertices[i].bitangent.z = static_cast<float>(m_pxmf3BiTangents[i].z);

				}
				delete[] m_pxmf3BiTangents;
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&nSubMeshes, sizeof(int), 1, pInFile);
			if (nSubMeshes > 0)
			{
				meshes.indices.resize(nSubMeshes);
				int* m_pnSubSetIndices = new int[nSubMeshes];
				UINT** m_ppnSubSetIndices = new UINT * [nSubMeshes];
				for (int i = 0; i < nSubMeshes; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile); //i
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(m_ppnSubSetIndices[i], sizeof(UINT), m_pnSubSetIndices[i], pInFile);

							for (int j = 0; j < m_pnSubSetIndices[i]; j++)
							{
								meshes.indices[i].push_back(static_cast<uint32>(m_ppnSubSetIndices[i][j]));
							}
						}
					}
				}
				delete[] m_pnSubSetIndices;
				for (int i = 0; i < nSubMeshes; i++) {
					delete[] m_ppnSubSetIndices[i];  // 각 내부 배열을 삭제
				}
				// 그 후, m_ppnSubSetIndices 배열을 삭제
				delete[] m_ppnSubSetIndices;
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}

void BinaryLoader::LoadMaterialFromFile(BinaryMeshInfo& meshes, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	int nMaterial = 0;
	UINT nReads = 0;

	int m_nMaterials = ReadIntegerFromFile(pInFile);
	meshes.materials.resize(m_nMaterials);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMaterial = ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			XMFLOAT4 m_xmf4AlbedoColor;
			nReads = (UINT)::fread(&m_xmf4AlbedoColor, sizeof(XMFLOAT4), 1, pInFile);

			meshes.materials[nMaterial].albedo = m_xmf4AlbedoColor;
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			XMFLOAT4 m_xmf4EmissiveColor;
			nReads = (UINT)::fread(&m_xmf4EmissiveColor, sizeof(XMFLOAT4), 1, pInFile);

			meshes.materials[nMaterial].emissive = m_xmf4EmissiveColor;
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			XMFLOAT4 m_xmf4SpecularColor;
			nReads = (UINT)::fread(&m_xmf4SpecularColor, sizeof(XMFLOAT4), 1, pInFile);

			meshes.materials[nMaterial].specular = m_xmf4SpecularColor;
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			float m_fGlossiness;
			nReads = (UINT)::fread(&m_fGlossiness, sizeof(float), 1, pInFile);

			meshes.materials[nMaterial].glossiness = m_fGlossiness;
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			float m_fSmoothness;
			nReads = (UINT)::fread(&m_fSmoothness, sizeof(float), 1, pInFile);

			meshes.materials[nMaterial].smoothness = m_fSmoothness;
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			float m_fMetallic;
			nReads = (UINT)::fread(&m_fMetallic, sizeof(float), 1, pInFile);

			meshes.materials[nMaterial].metalic = m_fMetallic;
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			float m_fSpecularHighlight;
			nReads = (UINT)::fread(&m_fSpecularHighlight, sizeof(float), 1, pInFile);

			meshes.materials[nMaterial].specularhighlight = m_fSpecularHighlight;
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			float m_fGlossyReflection;
			nReads = (UINT)::fread(&m_fGlossyReflection, sizeof(float), 1, pInFile);

			meshes.materials[nMaterial].glossyreflection = m_fGlossyReflection;
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			BYTE nStrLength = 64;
			UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);

			vector<char> buffer(nStrLength + 1, '\0'); // +1은 널 종료 문자 포함
			nReads = (UINT)::fread(buffer.data(), sizeof(char), nStrLength, pInFile); // 문자열 데이터 읽기

			string sTextureName;
			for (char c : buffer) {
				if (c == '\0') break;  // 널 종료 문자 만나면 중단
				if (c != '@') sTextureName += c; // @이 아닌 문자만 추가
			}

			wstring wTextureName = s2ws(sTextureName); // string → wstring 변환
			meshes.materials[nMaterial].albedoTexName = wTextureName;
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{

		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{

		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{

		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{

		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{

		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{

		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}

	}
}

void BinaryLoader::LoadSkinInfoFromFile(BinaryMeshInfo& meshes, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	char name[64] = {};
	::ReadStringFromFile(pInFile, name);		// Mesh의 이름 저장

	meshes.hasAnimation = true;
	// 임시
	XMFLOAT3 temp = {};
	
	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<BonesPerVertex>:"))
		{
			int nBonesPerVertex = ::ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&temp, sizeof(XMFLOAT3), 1, pInFile);

		}
		else if (!strcmp(pstrToken, "<BoneNames>:"))
		{
			int nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (nSkinningBones > 0)
			{
				_bones.resize(nSkinningBones);

				for (int i = 0; i < nSkinningBones; ++i) {
					char name[64] = {};
					_bones[i] = make_shared<BinaryBoneInfo>();
					::ReadStringFromFile(pInFile, name);
					string sboneName(name);
					_bones[i]->boneName = s2ws(sboneName);
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneOffsets>:"))
		{
			int nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (nSkinningBones > 0)
			{
				XMFLOAT4X4* BoneOffsets = new XMFLOAT4X4[nSkinningBones];
				for (int i = 0; i < nSkinningBones; ++i) {
					nReads = (UINT)::fread(&BoneOffsets[i], sizeof(XMFLOAT4X4), 1, pInFile);

					XMStoreFloat4x4(&_bones[i]->matOffset, XMMatrixTranspose(XMLoadFloat4x4(&BoneOffsets[i])));
				}
			delete[] BoneOffsets;
			}
		}
		else if (!strcmp(pstrToken, "<BoneIndices>:"))
		{
			int nVertices = ::ReadIntegerFromFile(pInFile);
			meshes.vertices.resize(nVertices);

			if (nVertices > 0)
			{
				XMINT4* m_pxmn4BoneIndices = new XMINT4[nVertices];

				nReads = (UINT)::fread(m_pxmn4BoneIndices, sizeof(XMINT4), nVertices, pInFile);
				
				for (UINT i = 0; i < nVertices; ++i)
				{
					meshes.vertices[i].indices.x = static_cast<float>(m_pxmn4BoneIndices[i].x);
					meshes.vertices[i].indices.y = static_cast<float>(m_pxmn4BoneIndices[i].y);
					meshes.vertices[i].indices.z = static_cast<float>(m_pxmn4BoneIndices[i].z);
					meshes.vertices[i].indices.w = static_cast<float>(m_pxmn4BoneIndices[i].w);
				}
				delete[] m_pxmn4BoneIndices;
			}
		}
		else if (!strcmp(pstrToken, "<BoneWeights>:"))
		{

			int nVertices = ::ReadIntegerFromFile(pInFile);
			if (nVertices > 0)
			{
				XMFLOAT4* m_pxmf4BoneWeights = new XMFLOAT4[nVertices];

				nReads = (UINT)::fread(m_pxmf4BoneWeights, sizeof(XMFLOAT4), nVertices, pInFile);
				for (UINT i = 0; i < nVertices; ++i)
				{
					meshes.vertices[i].weights.x = static_cast<float>(m_pxmf4BoneWeights[i].x);
					meshes.vertices[i].weights.y = static_cast<float>(m_pxmf4BoneWeights[i].y);
					meshes.vertices[i].weights.z = static_cast<float>(m_pxmf4BoneWeights[i].z);
					meshes.vertices[i].weights.w = static_cast<float>(m_pxmf4BoneWeights[i].w);
				}
				delete[] m_pxmf4BoneWeights;
			}
		}
		else if (!strcmp(pstrToken, "</SkinningInfo>"))
		{
			break;
		}
	}
}

void BinaryLoader::LoadAnimationFromFile(vector<BinaryMeshInfo>& meshes, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	vector<wstring> frameName;
	UINT nReads = 0;

	int32 animCount = 0;
	int32 frameCount = 0;
	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			animCount = ::ReadIntegerFromFile(pInFile);
			//_animClips.resize(animCount);
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			frameCount = ::ReadIntegerFromFile(pInFile);

			for (int j = 0; j < frameCount; j++)
			{
				::ReadStringFromFile(pInFile, pstrToken);
				string name(pstrToken);
				frameName.push_back(s2ws(name));
			}
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			int animIndex = ::ReadIntegerFromFile(pInFile);
			shared_ptr<BinaryAnimClipInfo> animClip = make_shared<BinaryAnimClipInfo>();

			::ReadStringFromFile(pInFile, pstrToken); //Animation Set Name
			string animName(pstrToken);

			float fLength = ::ReadFloatFromFile(pInFile);
			int nFramesPerSecond = ::ReadIntegerFromFile(pInFile);
			int nKeyFrames = ::ReadIntegerFromFile(pInFile);


			animClip->name = s2ws(animName);
			animClip->duration = fLength;;
			animClip->frameCount = nKeyFrames;
			animClip->keyFrames.resize(nKeyFrames); // 키프레임 개수만큼


			for (int i = 0; i < nKeyFrames; i++)
			{
				//animClip->keyFrames[i].resize(frameCount);
				animClip->keyFrames[i].resize(_bones.size());
				::ReadStringFromFile(pInFile, pstrToken);
				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					vector<BinaryKeyFrameInfo> keyFrameInfo(frameCount);
					int nKey = ::ReadIntegerFromFile(pInFile); //i
					float fKeyTime = ::ReadFloatFromFile(pInFile);

					XMFLOAT4X4* matrix = new XMFLOAT4X4[frameCount];
					for (int j = 0; j < frameCount; ++j)		
					{
						keyFrameInfo[j].time = fKeyTime;			// time
						nReads = (UINT)::fread(&matrix[j], sizeof(XMFLOAT4X4), 1, pInFile);

						keyFrameInfo[j].matTransform = XMLoadFloat4x4(&matrix[j]);		// matrix
						//animClip->keyFrames[i][j] = keyFrameInfo[j];

						meshes[j].transform->SetLocalMatrix(keyFrameInfo[j].matTransform);
						keyFrameInfo[j].matTransform = meshes[j].transform->GetToRootTransform();	// To-Root 행렬로 바꾼다.

				
						// 뼈 인덱스와 애니메이션 프레임 인덱스를 맞춰야한다. 이름으로 비교하면서
						for (int b = 0; b < _bones.size(); ++b)
						{
							if (frameName[j] == _bones[b]->boneName)
							{
								keyFrameInfo[j].boneName = frameName[j];
								animClip->keyFrames[i][b] = keyFrameInfo[j];
								break;
							}
						}
					}
					delete[] matrix;
				}
			}
			_animClips.push_back(animClip);

		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
		}
	}
}

void BinaryLoader::CreateTextures()
{
	for (size_t i = 0; i < _meshes.size(); i++)
	{
		for (size_t j = 0; j < _meshes[i].materials.size(); j++)
		{
			// Texture
			{
				wstring filename = _meshes[i].materials[j].albedoTexName;
				wstring fullPath = L"..\\Resources\\Texture\\" + filename + L".dds";
				if (filename.empty() == false)
					GET_SINGLE(Resources)->Load<Texture>(filename, fullPath);
			}
		}
	}
}

void BinaryLoader::CreateMaterials()
{
	for (size_t i = 0; i < _meshes.size(); i++)
	{
		if (_meshes[i].hasMesh)
		{
			for (size_t j = 0; j < _meshes[i].materials.size(); j++)
			{
				shared_ptr<Material> material = make_shared<Material>();
				wstring key = _meshes[i].meshName;
				material->SetName(key);
				material->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"Deferred"));

				{
					wstring albeodoTexName = _meshes[i].materials[j].albedoTexName;
					wstring key = albeodoTexName;
					shared_ptr<Texture> texture = GET_SINGLE(Resources)->Get<Texture>(key);
					if (texture)
						material->SetTexture(0, texture);
				}

				{
					float metalic = _meshes[i].materials[j].metalic;
					material->SetFloat(2, metalic);
				}
				GET_SINGLE(Resources)->Add<Material>(material->GetName(), material);
			}
		}
	}
}



