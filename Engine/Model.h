#pragma once
//インクルード
#include<string>
#include<vector>
#include"Transform.h"
#include"Fbx.h"

/// <summary>
/// 3Dモデル(FBXファイル)を管理する
/// </summary>
namespace Model
{
	struct ModelData {
		//FBX
		Fbx* pFbx_;
		//トランスフォーム
		Transform transform_;
		string filename_;	//ファイル名
		float nowFrame, animSpeed;
		int startFrame, endFrame;
		//▼初期化
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
	/// レイキャスト(レイを飛ばして当たり判定)
	/// </summary>
	/// <param name="handle">判定したいモデルの番号</param>
	/// <param name="data">必要なものをまとめたデータ</param>
	void RayCast(int handle, RayCastData* data);
};