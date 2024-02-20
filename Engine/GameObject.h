#pragma once
//�C���N���[�h
#include <list>
#include <string>
#include "Transform.h"
#include "Direct3D.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

using std::string;
using std::list;

/// <summary>
/// �S�ẴQ�[���I�u�W�F�N�g(�V�[�����܂߂�)���p������C���^�[�t�F�[�X
/// �Q�[���I�u�W�F�N�g�͐e�q�\���ɂȂ��Ă��āA
/// �}�g���N�X�̉e�����󂯂鎖�ɂȂ�
/// </summary>
class GameObject
{	
protected:
	Transform			transform_;
	string				objectName_;	//�I�u�W�F�N�g�̖��O�̔z��
	list<Collider*>		colliderList_;

public:
	GameObject();
	GameObject(GameObject* _parent);
	GameObject(GameObject* _parent, const std::string& _name);
	virtual ~GameObject();

	virtual void Initialize() =0;
	virtual void Update() =0;
	virtual void Draw() =0;
	virtual void Release() =0;
	void UpdateSub();
	void DrawSub();
	void ReleaseSub();
	XMMATRIX GetWorldMatrix();
	bool IsDead();
	void KillMe();
	void Enter();
	void Leave();
	void Visible();
	void Invisible();
	bool IsInitialized();
	void SetInitialized();
	bool IsEntered();
	bool IsVisibled();
	list<GameObject*>* GetChildList();
	GameObject* GetParent();
	GameObject* FindChildObject(string _objName);
	GameObject* FindObject(string _objName);
	const string& GetObjectName() const { return objectName_; }
	void PushBackChild(GameObject* _pObject);
	void PushFrontChild(GameObject* _pObject);
	void KillAllChildren();
	void AddCollider(Collider* _pCollider);
	virtual void OnCollision(GameObject* _pTarget) {};
	void ClearCollider();
	void Collision(GameObject* _pTarget);
	void CollisionDraw();
	//void RoundRobin(GameObject* _pTarget);
	GameObject* GetRootJob();

	//�Z�b�^�[�E�Q�b�^�[
	XMFLOAT3 GetPosition() { return transform_.position_; }
	XMFLOAT3 GetRotate() { return transform_.rotate_; }
	XMFLOAT3 GetScale() { return transform_.scale_; }
	XMFLOAT3 GetWorldPosition() { return Transform::Float3Add(GetParent()->transform_.position_, transform_.position_); }
	XMFLOAT3 GetWorldRotate() { return Transform::Float3Add(GetParent()->transform_.rotate_, transform_.rotate_); }
	XMFLOAT3 GetWorldScale() { return Transform::Float3Add(GetParent()->transform_.scale_, transform_.scale_); }
	void SetPosition(XMFLOAT3 _position);
	void SetPosition(float _x, float _y, float _z);
	void SetRotate(XMFLOAT3 _rotate) { transform_.position_ = _rotate; }
	void SetRotate(float _x, float _y, float _z) { SetRotate(XMFLOAT3(_x, _y, _z)); }
	void SetRotateX(float _x) { SetRotate(_x, transform_.rotate_.y, transform_.rotate_.z); }
	void SetRotateY(float _y) { SetRotate(transform_.rotate_.x, _y, transform_.rotate_.z); }
	void SetRotateZ(float _z) { SetRotate(transform_.rotate_.x, transform_.rotate_.y, _z); }
	void SetScale(XMFLOAT3 _scale) { transform_.scale_ = _scale; }
	void SetScale(float _x, float _y, float _z) { SetScale(XMFLOAT3(_x, _y, _z)); }
	void SetObjectName(string _s) { objectName_ = _s; }
	void SetTransform(Transform _transform) { transform_.position_ = _transform.position_; }

private:
	void KillObjectSub(GameObject* _pObject);

private:
	//�t���O
	struct OBJECT_STATE
	{
		unsigned initialized : 1;
		unsigned entered : 1;
		unsigned visible : 1;
		unsigned dead : 1;
	}state_;
	GameObject* pParent_;
	list<GameObject*>	childList_;
};

/// <summary>
/// �I�u�W�F�N�g���쐬����e���v���[�g
/// </summary>
/// <typeparam name="T">�e���v���[�g</typeparam>
/// <param name="_parent">�e�̃I�u�W�F�N�g</param>
/// <returns></returns>
template <class T>
T* Instantiate(GameObject* _pParent)
{
	T* pObject = new T(_pParent);
	if (_pParent != nullptr)
	{
		_pParent->PushBackChild(pObject);
	}
	pObject->Initialize();
	return pObject;
}

