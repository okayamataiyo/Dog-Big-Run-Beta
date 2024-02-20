#pragma once
#include <DirectXMath.h>

using namespace DirectX;

/// <summary>
/// 位置、向き、拡大率などを管理するクラス
/// </summary>
class Transform
{
public:
	XMMATRIX matTranslate_;	//移動行列
	XMMATRIX matRotate_;	//回転行列	
	XMMATRIX matScale_;		//拡大行列
	XMFLOAT3 position_;		//位置
	XMFLOAT3 rotate_;		//向き
	XMFLOAT3 scale_;		//拡大率
	Transform* pParent_;	//親オブジェクトのTransform

	//コンストラクタ
	Transform();

	//デストラクタ
	~Transform();

	//各行列の計算
	void Calclation();

	//ワールド行列を取得
	XMMATRIX GetWorldMatrix();

	//法線変形用行列を取得
	XMMATRIX GetNormalMatrix();

	static XMFLOAT3 Float3Add(XMFLOAT3 _a, XMFLOAT3 _b){return XMFLOAT3(_a.x + _b.x, _a.y + _b.y, _a.z + _b.z);}
};

inline XMFLOAT3 operator += (XMFLOAT3& _p, const XMVECTOR& _v)
{
	XMVECTOR pv = XMLoadFloat3(&_p);
	pv += _v;
	XMStoreFloat3(&_p, pv);
	return _p;
}

inline XMFLOAT3 operator +(const XMFLOAT3& _v1, const XMVECTOR& _v2)
{
	XMVECTOR pv = XMLoadFloat3(&_v1);
	pv += _v2;
	XMFLOAT3 out;
	XMStoreFloat3(&out, pv);
	return out;
}

inline float Length(XMVECTOR _vec)
{
	XMVECTOR lenVec = XMVector3Length(_vec);
	return XMVectorGetX(lenVec);
}

inline XMVECTOR operator -(const XMFLOAT3& _p1, const XMFLOAT3& _p2)
{
	XMVECTOR p1v = XMLoadFloat3(&_p1);
	XMVECTOR p2v = XMLoadFloat3(&_p2);
	return p1v - p2v;
}