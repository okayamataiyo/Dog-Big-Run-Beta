#pragma once
//�C���N���[�h
#include <DirectXMath.h>
#include "Direct3D.h"
#include "GameObject.h"

using namespace DirectX;

/// <summary>
/// �J����
/// </summary>
class Camera : public GameObject
{
private:
	XMFLOAT4 lightPos_;
	ID3D11Buffer* pCB_;
public:
	//�������i�v���W�F�N�V�����s��쐬�j
	void Initialize() override;

	//�X�V�i�r���[�s��쐬�j
	void Update(int _type);

	void Update() override;

	void Draw() override;
	
	void Release() override;

	void IntConstantBuffer(int _type);

	//���Z�b�^�[�E�Q�b�^�[
	void SetPosition(XMVECTOR _position,int _type);
	void SetPosition(XMFLOAT3 _position,int _type);
	void SetTarget(XMVECTOR _target,int _type);
	void SetTarget(XMFLOAT3 _target,int _type);
	XMVECTOR GetPosition(int _type);
	XMVECTOR GetTarget(int _type);
	XMFLOAT4 GetLightPos() { return(lightPos_); }
	XMMATRIX GetViewMatrix();											//�r���[�s����擾
	XMMATRIX GetProjectionMatrix();										//�v���W�F�N�V�����s����擾
};