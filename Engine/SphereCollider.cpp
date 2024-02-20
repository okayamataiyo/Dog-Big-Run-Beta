#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Model.h"

SphereCollider::SphereCollider(XMFLOAT3 _center, float _radius)
{
	center_ = _center;
	size_ = XMFLOAT3(_radius, _radius, _radius);
	type_ = COLLIDER_CIRCLE;

	//リリース時は判定枠は表示しない
#ifdef _DEBUG
	//テスト表示用判定枠
	hDebugModel_ = Model::Load("Assets/DebugCollision/SphereCollider.fbx");
#endif
}

bool SphereCollider::IsHit(Collider* _target)
{
	if (_target->type_ == COLLIDER_BOX)
		return IsHitBoxVsCircle((BoxCollider*)_target, this);
	else
		return IsHitCircleVsCircle((SphereCollider*)_target, this);
}