#pragma once
//インクルード
#include <string>
#include "Engine/GameObject.h"

using std::string;
//Stageを管理するクラス
class Stage : public GameObject
{
    int hModel_[2];    //モデル番号

    Transform FlyFloor;
public:
    //コンストラクタ
    //引数:parent 親オブジェクト(SceneManager)
    Stage(GameObject* _parent);

    //デストラクタ
    ~Stage();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;

    //指定した位置が通れるか通れないかを調べる
    //引数:x,z  調べる位置
    //戻り値:通れない=true/通れない=false
    //bool IsWall(int x, int z);

    int GetModelHandle(int _type) { return hModel_[_type]; }
};