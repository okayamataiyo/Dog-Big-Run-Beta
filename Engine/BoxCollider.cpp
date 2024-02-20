#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Model.h"

BoxCollider::BoxCollider(XMFLOAT3 _basePos, XMFLOAT3 _size)
{
	center_ = _basePos;
	size_ = _size;
	type_ = COLLIDER_BOX;

	//リリース時は判定枠は表示しない
#ifdef _DEBUG
	//テスト表示用判定枠
	hDebugModel_ = Model::Load("Assets/DebugCollision/BoxCollider.fbx");
#endif
}

bool BoxCollider::IsHit(Collider* _target)
{
	if (_target->type_ == COLLIDER_BOX)
		return IsHitBoxVsBox((BoxCollider*)_target, this);
	else
		return IsHitBoxVsCircle(this, (SphereCollider*)_target);
}