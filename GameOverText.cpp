#include "GameOverText.h"

GameOverText::GameOverText(GameObject* _pParent)
	:GameObject(_pParent, "GameOverText"),hModel_{-1}
{
}

void GameOverText::Initialize()
{
	hModel_ = Model::Load("Assets/GameOverText.fbx");
	assert(hModel_ >= 0);
	transform_.rotate_.y  = 180;
}

void GameOverText::Update()
{
}

void GameOverText::Draw()
{
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void GameOverText::Release()
{
}
