#pragma once
//インクルード
#include "Engine/GameObject.h"
#include "Engine/Camera.h"
#include "GameOverText.h"
class GameOverScene : public GameObject
{
	GameOverText* pGameOverText_;
	Camera* pCamera_;
	XMFLOAT3 camPos_;
public:
	GameOverScene(GameObject* _pParent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};

