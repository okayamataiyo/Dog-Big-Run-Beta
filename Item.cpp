//インクルード
#include "Engine/Model.h"
#include "Item.h"
#include "Stage.h"

Item::Item(GameObject* _parent)
	:GameObject(_parent, "Item"), hModel_(-1)
{

}

Item::~Item()
{
}

void Item::Initialize()
{
	hModel_ = Model::Load("Assets/Bone.fbx");
	assert(hModel_ >= 0);

	SphereCollider* pCollision = new SphereCollider(XMFLOAT3(0.0, 0.0, 0.0), 1);
	AddCollider(pCollision);

	transform_.scale_ = { 0.2,0.2,0.2 };
	transform_.position_ = { 10,0,0 };
}

void Item::Update()
{
	RayCastData data;
	Stage* pStage = (Stage*)FindObject("Stage");    //ステージオブジェクト
	int hStageModel[2];
	for (int i = 0u; i <= 1; i++)
	{
		hStageModel[i] = pStage->GetModelHandle(i);   //モデル番号を取得
		data.start = transform_.position_;  //レイの発射位置
		data.start.y = 0;
		data.dir = XMFLOAT3(0, -1, 0);       //レイの方向
		Model::RayCast(hStageModel[i], &data);  //レイを発射
		rayDist_ = data.dist;

		if (data.hit == true)
		{
			transform_.position_.y = -data.dist + 0.6;
		}
	}
}

void Item::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Item::Release()
{
}

void Item::OnCollision(GameObject* _pTarget)
{
	if (_pTarget->GetObjectName() == "PlayerFirst")
	{
		
	}
}
