#pragma once
//�C���N���[�h
#include "GameObject.h"

/// <summary>
/// ��ԏ�ɗ���I�u�W�F�N�g
/// �S�ẴI�u�W�F�N�g�́A����̎q���ɂȂ�
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

