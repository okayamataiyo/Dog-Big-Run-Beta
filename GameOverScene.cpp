#include "Engine/Input.h"
#include "Engine/SceneManager.h"
#include "Engine/Direct3D.h"
#include "GameOverScene.h"
#include "Sky.h"
GameOverScene::GameOverScene(GameObject* _pParent)
	:GameObject(_pParent, "GameOverScene")
{

}

void GameOverScene::Initialize()
{
	pGameOverText_ = Instantiate<GameOverText>(this);
	Instantiate<Sky>(this);
	pCamera_ = new Camera;
}

void GameOverScene::Update()
{
	camPos_ = pGameOverText_->GetPosition();
	camPos_.y += 2;
	camPos_.z -= 15;
	pCamera_->SetPosition(camPos_, 0);
	pCamera_->SetTarget(pGameOverText_->GetPosition(), 0);
	if (Input::IsKeyDown(DIK_B))
	{
		Direct3D::SetIsChangeView(2);
		SceneManager* pSceneManager = (SceneManager*)FindObject("SceneManager");
		pSceneManager->ChangeScene(SCENE_ID_PLAY);
	}
}

void GameOverScene::Draw()
{

}

void GameOverScene::Release()
{
}
