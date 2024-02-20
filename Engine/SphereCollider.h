#pragma once
//インクルード
#include "Collider.h"

/// <summary>
/// 球体の当たり判定
/// </summary>
class SphereCollider : public Collider
{
	//Colliderクラスのprivateメンバにアクセスできるようにする
	friend class Collider;

public:
	//コンストラクタ（当たり判定の作成）
	//引数：basePos	当たり判定の中心位置（ゲームオブジェクトの原点から見た位置）
	//引数：radius	当たり判定のサイズ（半径）
	SphereCollider(XMFLOAT3 _center, float _radius);

private:
	/// <summary>
	/// 接触判定
	/// </summary>
	/// <param name="_target">相手の当たり判定</param>
	/// <returns>接触していればtrue</returns>
	bool IsHit(Collider* _target) override;
};

