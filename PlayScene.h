#pragma once
//インクルード
#include "Engine/GameObject.h"
#include "Engine/Camera.h"
#include "Player.h"
#include "Sky.h"
/// <summary>
/// 遊べるシーン
/// </summary>
class PlayScene : public GameObject
{
private:
    //メンバ変数
    Player* pPlayer_[2];
    Sky* pSky_;
    Camera* pCamera_;

    XMFLOAT3 camVec_[2];
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="pParent">親オブジェクト</param>
    PlayScene(GameObject* _pParent);
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
};

