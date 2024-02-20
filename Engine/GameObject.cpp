#include "GameObject.h"

GameObject::GameObject()
	:GameObject(nullptr, "")
{
}

GameObject::GameObject(GameObject* _parent)
	:GameObject(_parent, "")
{
}

GameObject::GameObject(GameObject* _parent, const string& _name)
	: pParent_(_parent), objectName_(_name)
{
	childList_.clear();
	state_ = { 0,1,1,0 };
	if (pParent_ != nullptr) {
		this->transform_.pParent_ = &(_parent->transform_);
	}
}

GameObject::~GameObject()
{
	for (auto it = colliderList_.begin(); it != colliderList_.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	colliderList_.clear();
}

bool GameObject::IsDead()
{
	return(state_.dead != 0);
}

void GameObject::KillMe()
{
	state_.dead = 1;
}

void GameObject::Enter()
{
	state_.entered = 1;
}

void GameObject::Leave()
{
	state_.entered = 0;
}

void GameObject::Visible()
{
	state_.visible = 1;
}

void GameObject::Invisible()
{
	state_.visible = 0;
}

bool GameObject::IsInitialized()
{
	return (state_.initialized != 0);
}

void GameObject::SetInitialized()
{
	state_.initialized = 1;
}

bool GameObject::IsEntered()
{
	return (state_.entered != 0);
}

bool GameObject::IsVisibled()
{
	return (state_.visible != 0);
}

list<GameObject*>* GameObject::GetChildList()
{
	return &childList_;
}

void GameObject::SetPosition(XMFLOAT3 _position)
{
	transform_.position_ = _position;
}

void GameObject::SetPosition(float _x, float _y, float _z)
{
	SetPosition(XMFLOAT3(_x, _y, _z));
}

void GameObject::KillObjectSub(GameObject* _pObject)
{
	if (!childList_.empty())
	{
		auto list = _pObject->GetChildList();
		auto it = list->begin();
		auto end = list->end();
		while (it != end)
		{
			KillObjectSub(*it);
			delete* it;
			it = list->erase(it);
		}
		list->clear();
	}
	_pObject->Release();
}

GameObject* GameObject::GetParent()
{
	return pParent_;
}

GameObject* GameObject::FindChildObject(string _objName)
{
	if (_objName == this->objectName_) {

		return(this);	//自分が_objNameのオブジェクトだった！
	}
	else {

//		for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {
		for(auto itr: childList_){

			GameObject* obj = itr->FindChildObject(_objName);
			if (obj != nullptr)
				return obj;
		}
	}
	return nullptr;
}

GameObject* GameObject::GetRootJob()
{
	if (pParent_ == nullptr)
	{
		return this;
	}
	return pParent_->GetRootJob();
}

GameObject* GameObject::FindObject(string _objName) 
{
	GameObject* rootJob = GetRootJob();
	GameObject* result = rootJob->FindChildObject(_objName);
	return (result);
	//return GetRootJob()->FindObject(_objName);
}

void GameObject::PushBackChild(GameObject* _pObject)
{
	assert(_pObject != nullptr);

	_pObject->pParent_ = this;
	childList_.push_back(_pObject);
}

void GameObject::PushFrontChild(GameObject* _pObject)
{
	assert(_pObject != nullptr);
	_pObject->pParent_ = this;
	childList_.push_front(_pObject);
}

void GameObject::UpdateSub()
{
	Update();
	Transform();
	//RoundRobin(GetRootJob());
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {

		(*itr)->UpdateSub();
	}

	for (auto itr = childList_.begin(); itr != childList_.end();) {

		if ((*itr)->IsDead() == true) {

			(*itr)->ReleaseSub();
			SAFE_DELETE(*itr);				//自分自身を消す
			itr = childList_.erase(itr);	//リストからも削除
		}
		else {
			(*itr)->Collision(GetParent());
			itr++;
		}
	}
}

void GameObject::DrawSub()
{
	Draw();

#ifdef _DEBUG
	if (Direct3D::isDrawCollision_)
	{
		CollisionDraw();
	}
#endif

	for (auto itr = childList_.begin(); itr != childList_.end(); itr++)
		(*itr)->DrawSub();
}

void GameObject::ReleaseSub()
{
	//コライダーを削除
	ClearCollider();
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {

		(*itr)->ReleaseSub();	//*itrのリリースを呼ぶ
		SAFE_DELETE(*itr);		//*itr自体を消す
	}

	Release();
}

XMMATRIX GameObject::GetWorldMatrix()
{
	return transform_.GetWorldMatrix();
}

void GameObject::KillAllChildren()
{
	if (childList_.empty())
	{
		return;
	}

	auto it = childList_.begin();
	auto end = childList_.end();

	while (it != end)
	{
		KillObjectSub(*it);
		delete* it;
		it = childList_.erase(it);
	}
	childList_.clear();
}

void GameObject::AddCollider(Collider* _pCollider)
{

	_pCollider->SetGameObject(this);
	colliderList_.push_back(_pCollider);

}

void GameObject::ClearCollider()
{
	for (auto itr = colliderList_.begin(); itr != colliderList_.end(); itr++)
	{
		SAFE_DELETE(*itr);
	}
	colliderList_.clear();
}

void GameObject::Collision(GameObject* _pTarget)
{
	if (_pTarget == this || _pTarget == nullptr)
	{
		return;		//ターゲットにコライダーがアタッチされていない
	}
	
	//自分とターゲットの距離
	//もし、自分のコライダーとターゲットがぶつかっていたら
	//onCollision(pTarget)を呼び出す
	for (auto i = this->colliderList_.begin(); i != this->colliderList_.end(); i++)
	{
		for (auto j = _pTarget->colliderList_.begin(); j != _pTarget->colliderList_.end(); j++)
		{
			if ((*i)->IsHit(*j))
			{
				this->OnCollision(_pTarget);
			}
		}
	}

	if (_pTarget->childList_.empty())
	{
		return;
	}

	for (auto i = _pTarget->childList_.begin(); i != _pTarget->childList_.end(); i++)
	{
		Collision(*i);
	}
}

//void GameObject::RoundRobin(GameObject* _pTarget)
//{
//
//	if (pCollider_ == nullptr)
//		return;
//	if (_pTarget->pCollider_ != nullptr)	//自分とターゲット
//		Collision(_pTarget);
//	//自分の子供全部とターゲット
//	for (auto itr:_pTarget->childList_)
//		RoundRobin(itr);
//}

void GameObject::CollisionDraw()
{
	Direct3D::SetShader(SHADER_UNLIT);
	for (auto i = this->colliderList_.begin(); i != this->colliderList_.end(); i++)
	{
		(*i)->Draw(GetWorldPosition());
	}
	Direct3D::SetShader(SHADER_3D);
}
