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
	int Load(std::string fileName);
	void SetTransform(int hModel, Transform transform);
	void Draw(int hModel);
	void Release();

	/// <summary>
	/// レイキャスト(レイを飛ばして当たり判定)
	/// </summary>
	/// <param name="handle">判定したいモデルの番号</param>
	/// <param name="data">必要なものをまとめたデータ</param>
	void RayCast(int handle, RayCastData* data);
};