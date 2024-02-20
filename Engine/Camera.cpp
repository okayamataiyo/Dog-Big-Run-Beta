#include "Camera.h"

XMVECTOR position_[2];	//�J�����̈ʒu�i���_�j
XMVECTOR target_[2];	//����ʒu�i�œ_�j
XMMATRIX viewMatrix_;	//�r���[�s��
XMMATRIX projMatrix_;	//�v���W�F�N�V�����s��

struct CAMBUFF
{
	XMFLOAT4 lightPos;
	XMFLOAT4 eyePos[2];
	XMFLOAT4 target[2];
};


//������
void Camera::Initialize()
{
	for (int i = 0; i <= 1; i++)
	{
		position_[i] = XMVectorSet(0, 3 * i, -20, 0);	//�J�����̈ʒu
		target_[i] = XMVectorSet(0, 0, 0, 0);	//�J�����̏œ_
		IntConstantBuffer(i);
	}
	//�v���W�F�N�V�����s��
	projMatrix_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)800 / (FLOAT)600, 0.1f, 8000.0f);
	lightPos_ = { 0,2,-1,0 };

}

//�X�V
void Camera::Update(int _type)
{
	//�r���[�s��̍쐬
	viewMatrix_ = XMMatrixLookAtLH(position_[_type], target_[_type], XMVectorSet(0, 1, 0, 0));
	IntConstantBuffer(_type);

}

void Camera::Update()
{
}

void Camera::Draw()
{
}

void Camera::Release()
{
}

void Camera::IntConstantBuffer(int _type)
{
	//�R���X�^���g�o�b�t�@�ɏ���n��
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CAMBUFF);
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	//�R���X�^���g�o�b�t�@�̍쐬
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pCB_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
	}

	CAMBUFF cs;
	cs.lightPos = lightPos_;
	XMStoreFloat4(&cs.eyePos[_type], GetPosition(_type));
	XMStoreFloat4(&cs.target[_type], GetTarget(_type));
	Direct3D::pContext_->UpdateSubresource(pCB_, 0, NULL, &cs, 0, 0);
	Direct3D::pContext_->VSSetConstantBuffers(1, 1, &pCB_);//���_�V�F�[�_�[
	Direct3D::pContext_->PSSetConstantBuffers(1, 1, &pCB_);//�s�N�Z���V�F�[�_�[
}

//�ʒu��ݒ�
void Camera::SetPosition(XMVECTOR _position, int _type)
{
	position_[_type] = _position;
}

void Camera::SetPosition(XMFLOAT3 _position, int _type)
{
	SetPosition(XMLoadFloat3(&_position), _type);
}

//�œ_��ݒ�
void Camera::SetTarget(XMVECTOR _target, int _type)
{
	target_[_type] = _target;
}

void Camera::SetTarget(XMFLOAT3 _target, int _type)
{
	SetTarget(XMLoadFloat3(&_target), _type);
}

XMVECTOR Camera::GetPosition(int _type)
{
	return position_[_type];
}

XMVECTOR Camera::GetTarget(int _type)
{
	return target_[_type];
}

//�r���[�s����擾
XMMATRIX Camera::GetViewMatrix()
{
	return viewMatrix_;
}

//�v���W�F�N�V�����s����擾
XMMATRIX Camera::GetProjectionMatrix()
{
	return projMatrix_;
}