#include "Engine/Model.h"
#include "Engine/Camera.h"
#include "Engine/Input.h"
#include "Engine/Fbx.h"
#include "Stage.h"

Stage::Stage(GameObject* _parent)
    :GameObject(_parent, "Stage"),hModel_{-1,-1}
{

}

Stage::~Stage()
{

}

void Stage::Initialize()
{
    //モデルデータのロード
    hModel_[0] = Model::Load("Assets/Ground.fbx");
    assert(hModel_[0] >= 0);
    hModel_[1] = Model::Load("Assets/Floor.fbx");
    assert(hModel_[1] >= 0);
    transform_.position_.y = -5;
    transform_.rotate_.y = 90;
    transform_.scale_ = { 4,1,4 };
    FlyFloor.position_ = { 3,0,3 };
}

void Stage::Update()
{

}

void Stage::Draw()
{
    for (int i = 0u; i <= 1; i++)
    {
        Model::SetTransform(hModel_[0], transform_);
        Model::SetTransform(hModel_[1], FlyFloor);
        //最初に3Dで描画後、枠づけも描画
        for (int j = 3; j >= 1; j -= 2)
        {
            Direct3D::SetShader(static_cast<SHADER_TYPE>(j));
            Model::Draw(hModel_[i]);
        }
    }
}

void Stage::Release()
{
}