#pragma once
#include <DirectXMath.h>

using namespace DirectX;

/// <summary>
/// �ʒu�A�����A�g�嗦�Ȃǂ��Ǘ�����N���X
/// </summary>
class Transform
{
public:
	XMMATRIX matTranslate_;	//�ړ��s��
	XMMATRIX matRotate_;	//��]�s��	
	XMMATRIX matScale_;		//�g��s��
	XMFLOAT3 position_;		//�ʒu
	XMFLOAT3 rotate_;		//����
	XMFLOAT3 scale_;		//�g�嗦
	Transform* pParent_;	//�e�I�u�W�F�N�g��Transform

	//�R���X�g���N�^
	Transform();

	//�f�X�g���N�^
	~Transform();

	//�e�s��̌v�Z
	void Calclation();

	//���[���h�s����擾
	XMMATRIX GetWorldMatrix();

	//�@���ό`�p�s����擾
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