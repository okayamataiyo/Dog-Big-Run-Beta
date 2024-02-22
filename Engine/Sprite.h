#pragma once
//インクルード
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <vector>
#include "Texture.h"
#include "Transform.h"

using namespace DirectX;
using std::vector;

/// <summary>
/// 2D画像を表示するためのクラス
/// </summary>
class Sprite
{
private:
	//コンスタントバッファー
	struct CONSTANT_BUFFER
	{
		XMMATRIX world;
		XMMATRIX uvTrans;
		XMFLOAT4 color;
	};
	//1つの頂点情報を格納する構造体
	struct VERTEX
	{
		XMFLOAT3 position;
		XMFLOAT3 uv;
	};
protected:
	UINT64 vertexNum_;				//頂点数
	ID3D11Buffer* pVertexBuffer_;

	UINT64 indexNum;
	vector<int>index_;
	ID3D11Buffer* pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;
	Texture* pTexture_;

public:
	Sprite();
	~Sprite();
	HRESULT Load(string fileName);               //テクスチャをロード
	void Draw(Transform& _transform, RECT _rect, float _alpha);
	void Release();
	XMFLOAT2 GetTextureSize() { return pTexture_->GetSize(); }
	void InitVertex();
	void InitIndex();        //インデックス情報を準備
	void InitConstantBuffer();
};

