#pragma once
//�C���N���[�h
#include "Collider.h"

/// <summary>
/// ���^�̓����蔻��
/// </summary>
class BoxCollider : public Collider
{
	//Collider�N���X��private�����o�ɃA�N�Z�X�ł���悤�ɂ���
	friend class Collider;

public:

	/// <summary>
	/// �R���X�g���N�^(�����蔻��̍쐬)
	/// </summary>
	/// <param name="basePos">�����蔻��̒��S�ʒu�i�Q�[���I�u�W�F�N�g�̌��_���猩���ʒu)</param>
	/// <param name="size">	//�����Fsize	�����蔻��̃T�C�Y�i���A�����A���s���j</param>
	BoxCollider(XMFLOAT3 _basePos, XMFLOAT3 _size);

private:

	/// <summary>
	/// �ڐG����
	/// </summary>
	/// <param name="target">����̓����蔻��</param>
	/// <returns>�ڐG���Ă��true</returns>
	bool IsHit(Collider* _target) override;
};

