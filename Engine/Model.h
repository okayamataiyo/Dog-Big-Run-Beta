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
	struct ModelData {
		//FBX
		Fbx* pFbx_;
		//�g�����X�t�H�[��
		Transform transform_;
		string filename_;	//�t�@�C����
		float nowFrame, animSpeed;
		int startFrame, endFrame;
		//��������
		ModelData() :pFbx_(nullptr), nowFrame(0), startFrame(0), endFrame(0), animSpeed(0)
		{
		}

		void SetAnimFrame(int _start, int _end, float _speed)
		{
			nowFrame = (float)_start;
			startFrame = _start;
			endFrame = _end;
			animSpeed = _speed;
		}
	};

	int Load(std::string fileName);
	void SetAnimFrame(int _hModel, int _startFrame, int _endFrame, float _animSpeed);
	void SetTransform(int hModel, Transform transform);
	int GetAnimFrame(int _hModel_);
	XMFLOAT3 GetBonePosition(int _hModel, string _boneName);
	void Draw(int _hModel);
	void Release();

	/// <summary>
	/// ���C�L���X�g(���C���΂��ē����蔻��)
	/// </summary>
	/// <param name="handle">���肵�������f���̔ԍ�</param>
	/// <param name="data">�K�v�Ȃ��̂��܂Ƃ߂��f�[�^</param>
	void RayCast(int handle, RayCastData* data);
};