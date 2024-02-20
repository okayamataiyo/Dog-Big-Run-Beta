#pragma once
//�C���N���[�h
#include "Collider.h"

/// <summary>
/// ���̂̓����蔻��
/// </summary>
class SphereCollider : public Collider
{
	//Collider�N���X��private�����o�ɃA�N�Z�X�ł���悤�ɂ���
	friend class Collider;

public:
	//�R���X�g���N�^�i�����蔻��̍쐬�j
	//�����FbasePos	�����蔻��̒��S�ʒu�i�Q�[���I�u�W�F�N�g�̌��_���猩���ʒu�j
	//�����Fradius	�����蔻��̃T�C�Y�i���a�j
	SphereCollider(XMFLOAT3 _center, float _radius);

private:
	/// <summary>
	/// �ڐG����
	/// </summary>
	/// <param name="_target">����̓����蔻��</param>
	/// <returns>�ڐG���Ă����true</returns>
	bool IsHit(Collider* _target) override;
};

