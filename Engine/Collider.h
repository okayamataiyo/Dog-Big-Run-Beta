#pragma once
//�C���N���[�h
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

//�N���X�̑O���錾
class GameObject;
class BoxCollider;
class SphereCollider;


//�����蔻��̃^�C�v
enum ColliderType
{
	COLLIDER_BOX,		//���^
	COLLIDER_CIRCLE		//����
};

/// <summary>
/// �����蔻����Ǘ�����N���X
/// </summary>
class Collider
{
	//���ꂼ��̃N���X��private�����o�ɃA�N�Z�X�ł���悤�ɂ���
	friend class BoxCollider;
	friend class SphereCollider;

protected:
	GameObject* pGameObject_;		//���̔���������Q�[���I�u�W�F�N�g
	ColliderType	type_;			//���
	XMFLOAT3		center_;		//���S�ʒu�i�Q�[���I�u�W�F�N�g�̌��_���猩���ʒu�j
	XMFLOAT3		size_;			//����T�C�Y�i���A�����A���s���j
	int				hDebugModel_;	//�f�o�b�O�\���p�̃��f����ID

public:
	//�R���X�g���N�^
	Collider();

	//�f�X�g���N�^
	virtual ~Collider();

	/// <summary>
	/// �ڐG����(�p�����SphereCollider��BoxCollider�ŃI�[�o�[���C�h)
	/// </summary>
	/// <param name="_target">����̓����蔻��</param>
	/// <returns>�ڐG���Ă����true</returns>
	virtual bool IsHit(Collider* _target) = 0;

	/// <summary>
	/// ���^���m�̏Փ˔���
	/// </summary>
	/// <param name="_boxA">1�ڂ̔��^����</param>
	/// <param name="_boxB">2�ڂ̔��^����</param>
	/// <returns>�ڐG���Ă����true</returns>
	bool IsHitBoxVsBox(BoxCollider* _boxA, BoxCollider* _boxB);

	/// <summary>
	/// ���^�Ƌ��̂̏Փ˔���
	/// </summary>
	/// <param name="_box">1�ڂ̔��^����</param>
	/// <param name="_sphere">2�ڂ̋��̔���</param>
	/// <returns>�ڐG���Ă����true</returns>
	bool IsHitBoxVsCircle(BoxCollider* _box, SphereCollider* _sphere);

	/// <summary>
	/// ���̓����̏Փ˔���
	/// </summary>
	/// <param name="_circleA">1�ڂ̋��̔���</param>
	/// <param name="_circleB">2�ڂ̋��̔���</param>
	/// <returns>�ڐG���Ă����true</returns>
	bool IsHitCircleVsCircle(SphereCollider* _circleA, SphereCollider* _circleB);

	/// <summary>
	/// �e�X�g�\���p�̘g��`��
	/// </summary>
	/// <param name="_position">�I�u�W�F�N�g�̈ʒu</param>
	void Draw(XMFLOAT3 _position);

	//���Z�b�^�[�E�Q�b�^�[
	void SetGameObject(GameObject* _gameObject) { pGameObject_ = _gameObject; }

};

