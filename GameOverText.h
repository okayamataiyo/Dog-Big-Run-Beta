#pragma once
//インクルード
#include "Engine/Model.h"
#include "Engine/GameObject.h"

class GameOverText : public GameObject
{
private:
	int hModel_;
public:
	GameOverText(GameObject* _pParent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};

