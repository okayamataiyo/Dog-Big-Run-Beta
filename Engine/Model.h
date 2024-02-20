#pragma once
//�C���N���[�h
#include<string>
#include<vector>
#include"Transform.h"
#include"Fbx.h"

/// <summary>
/// 3D���f��(FBX�t�@�C��)���Ǘ�����
/// </summary>
namespace Model
{
	int Load(std::string fileName);
	void SetTransform(int hModel, Transform transform);
	void Draw(int hModel);
	void Release();

	/// <summary>
	/// ���C�L���X�g(���C���΂��ē����蔻��)
	/// </summary>
	/// <param name="handle">���肵�������f���̔ԍ�</param>
	/// <param name="data">�K�v�Ȃ��̂��܂Ƃ߂��f�[�^</param>
	void RayCast(int handle, RayCastData* data);
};