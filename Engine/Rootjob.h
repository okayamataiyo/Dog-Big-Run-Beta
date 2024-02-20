#pragma once
//インクルード
#include "GameObject.h"

/// <summary>
/// 一番上に来るオブジェクト
/// 全てのオブジェクトは、これの子孫になる
/// </summary>
class Rootjob : public GameObject
{
public:
    Rootjob();
    ~Rootjob();

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
};

