#include "Sky.h"
#include "Engine/Model.h"

Sky::Sky(GameObject* _parent)
	:GameObject(_parent, "Sky"),hModel_(-1)
{

}

Sky::~Sky()
{
}

void Sky::Initialize()
{
	//モデルデータのロード
	hModel_ = Model::Load("Assets/Sky.fbx");
	assert(hModel_ >= 0);
	//transform_.scale_ = { 0.5,0.5,0.5 };
	transform_.position_.y = -50;
}

void Sky::Update()
{
	transform_.rotate_.y += 0.01;
}

void Sky::Draw()
{
	//最初に2Dで描画後、枠づけも描画
	Model::SetTransform(hModel_, transform_);
	for (int i = 0; i <= 1; i++)
	{
		Direct3D::SetShader(static_cast<SHADER_TYPE>(i));
		Model::Draw(hModel_);
	}
}

void Sky::Release()
{
}
