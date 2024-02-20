#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include <vector>
#include "Transform.h"
#include "Camera.h"

#pragma comment(lib, "LibFbxSDK-MD.lib")
#pragma comment(lib, "LibXml2-MD.lib")
#pragma comment(lib, "zlib-MD.lib")

using std::vector;

class Texture;
class Camera;

extern Camera* pCamera;

//レイキャスト構造体
struct RayCastData
{
	XMFLOAT3 start;		//レイ発射位置
	XMFLOAT3 dir;		//レイの向きベクトル
	float	 dist;		//衝突点までの距離
	BOOL	 hit;		//レイが当たったか
	XMFLOAT3 normal;	//法線

	RayCastData() { dist = 99999.0f; }
};

class Fbx
{
	//マテリアル
	struct MATERIAL
	{
		Texture* pTexture;
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT4 specular;
		float shineness;
	}*pMaterial_;

	//コンスタントバッファー
	struct CONSTANT_BUFFER_MODEL {
		XMMATRIX matWVP;			//wvp
		XMMATRIX matW;				//wvp
		XMMATRIX matNormal;			//ワールド変換だけのやつ
		XMFLOAT4 diffuseColor;		//面の色
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
		int*     pBoneIndex;
		float* pBoneWeight;
	};

	//頂点情報
	struct VERTEX {
		XMVECTOR position;				//頂点数
		XMVECTOR uv;					//ポリゴン数
		XMVECTOR normal;				//マテリアルの個数
	};

	int vertexCount_;					//頂点数	FBXファイルを扱うために必要になる変数を宣言する。
	int polygonCount_;					//ポリゴン数
	int materialCount_;					//マテリアルの個数

	FbxScene* pFbxScene_;				//FBXファイルのシーン(Mayaで作った全ての物体)を扱う
	ID3D11Buffer*  pVertexBuffer_;		//頂点バッファ
	ID3D11Buffer** pIndexBuffer_;		//インデックスバッファ
	ID3D11Buffer*  pConstantBuffer_;	//コンスタントバッファ
	MATERIAL*      pMaterialList_;
	vector<int>    indexCount_;
	vector<Fbx*>   parts_;
//	int* indexCount_;

	//ボーン制御情報
	FbxSkin*       pSkinInfo_;
	FbxCluster**   ppCluster_;
	int            numBone_;
	Bone*          pBoneArray_;
	Weight*        pWeightArray_;

	void InitVertex(fbxsdk::FbxMesh* _mesh);
	void InitIndex(fbxsdk::FbxMesh* _mesh);
	void InitConstantBuffer();
	void InitMaterial(fbxsdk::FbxNode* _pNode);
	void InitSkelton(FbxMesh* _pMesh);
	bool IsFloatColor_;
	XMFLOAT4 dColor_;
	Texture* pToonTex_;
	VERTEX* pVertexData_;
	DWORD** ppIndexData_;
	FbxTime::EMode frameRate_;
	float		   nowFrame_,animSpeed_;
	int			   startFrame_, endFrame_;

public:
	//メンバ関数
	Fbx();
	HRESULT Load(std::string _fileName);
	void Draw(Transform& _transform,int _frame);
	void DrawSkinAnime(Transform& _transform, FbxTime _time);
	void DrawMeshAnime(Transform& _transform, FbxTime _time, FbxScene* _scene);
	bool GetBonePosition(string _boneName, XMFLOAT3* _position);
	FbxSkin* GetSkinInfo() { return pSkinInfo_; }
	void Release();
	
	/// <summary>
	/// レイキャスト(レイを飛ばして当たり判定)
	/// </summary>
	/// <param name="data">必要なものをまとめたデータ</param>
	void RayCast(RayCastData* data);
};

