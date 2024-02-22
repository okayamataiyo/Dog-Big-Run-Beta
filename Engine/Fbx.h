#pragma once
//�C���N���[�h
#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include <vector>
#include "Transform.h"
#include "Camera.h"
//�����J
#pragma comment(lib, "LibFbxSDK-MD.lib")
#pragma comment(lib, "LibXml2-MD.lib")
#pragma comment(lib, "zlib-MD.lib")

using std::vector;

class Texture;
class Camera;

extern Camera* pCamera;

//���C�L���X�g�\����
struct RayCastData
{
	XMFLOAT3 start;		//���C���ˈʒu
	XMFLOAT3 dir;		//���C�̌����x�N�g��
	float	 dist;		//�Փ˓_�܂ł̋���
	BOOL	 hit;		//���C������������
	XMFLOAT3 normal;	//�@��

	RayCastData() { dist = 99999.0f; }
};

/// <summary>
/// FBX�t�@�C���������N���X
/// </summary>
class Fbx
{
	//�}�e���A��
	struct MATERIAL
	{
		DWORD polygonCount;
		Texture* pTexture;
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT4 specular;
		float shineness;
	}*pMaterialList_;

	//�R���X�^���g�o�b�t�@�[
	struct CONSTANT_BUFFER_MODEL {
		XMMATRIX matWVP;			//wvp
		XMMATRIX matW;				//wvp
		XMMATRIX matNormal;			//���[���h�ϊ������̂��
		XMFLOAT4 diffuseColor;		//�ʂ̐F
		XMFLOAT4 ambientColor;
		XMFLOAT4 specularColor;
		float  shineness;
		BOOL isTextured;
	};

	struct Bone
	{
		XMMATRIX bindPose;
		XMMATRIX newPose;
		XMMATRIX diffPose;
	};

	struct Weight
	{
		XMFLOAT3 posOrigin;
		XMFLOAT3 normalOrigin;
		int* pBoneIndex;
		float* pBoneWeight;
	};

	//���_���
	struct VERTEX {
		XMVECTOR position;				//���_��
		XMVECTOR uv;					//�|���S����
		XMVECTOR normal;				//�}�e���A���̌�
	};

	int vertexCount_;					//���_��	FBX�t�@�C�����������߂ɕK�v�ɂȂ�ϐ���錾����B
	int polygonCount_;					//�|���S����
	int materialCount_;					//�}�e���A���̌�
	int polygonVertexCount_;

	FbxScene* pFbxScene_;				//FBX�t�@�C���̃V�[��(Maya�ō�����S�Ă̕���)������
	ID3D11Buffer* pVertexBuffer_;		//���_�o�b�t�@
	ID3D11Buffer** pIndexBuffer_;		//�C���f�b�N�X�o�b�t�@
	ID3D11Buffer* pConstantBuffer_;	//�R���X�^���g�o�b�t�@
	vector<int>    indexCount_;
	vector<Fbx*>   parts_;
	
	//�{�[��������
	FbxSkin* pSkinInfo_;
	FbxCluster** ppCluster_;
	int            numBone_;
	Bone* pBoneArray_;
	Weight* pWeightArray_;

	void InitVertex(fbxsdk::FbxMesh* _mesh);
	void InitIndex(fbxsdk::FbxMesh* _mesh);
	void InitConstantBuffer();
	void InitMaterial(fbxsdk::FbxNode* _pNode);
	void InitTexture(fbxsdk::FbxSurfaceMaterial* _pMaterial, const DWORD& _i);
	void InitSkelton(FbxMesh* _pMesh);
	bool IsFloatColor_;
	XMFLOAT4 dColor_;
	VERTEX* pVertexData_;
	DWORD** ppIndexData_;
	FbxTime::EMode frameRate_;
	float		   nowFrame_, animSpeed_;
	int			   startFrame_, endFrame_;

public:
	//�����o�֐�
	Fbx();
	HRESULT Init(FbxNode* _pNode);
	HRESULT Load(std::string _fileName);
	void Draw(Transform& _transform, int _frame);
	void Draw(Transform& _transform);
	void DrawSkinAnime(Transform& _transform, FbxTime _time);
	void DrawMeshAnime(Transform& _transform, FbxTime _time, FbxScene* _scene);
	XMFLOAT3 GetBonePosition(string _boneName);
	bool GetBonePosition(string _boneName, XMFLOAT3* _position);
	FbxSkin* GetSkinInfo() { return pSkinInfo_; }
	void CheckNode(FbxNode* _pNode, vector<Fbx*>* _pPartsList);
	void Release();
	
	/// <summary>
	/// ���C�L���X�g(���C���΂��ē����蔻��)
	/// </summary>
	/// <param name="data">�K�v�Ȃ��̂��܂Ƃ߂��f�[�^</param>
	void RayCast(RayCastData* data);
};

