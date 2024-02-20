#pragma once
//インクルード
#include "Collider.h"

/// <summary>
/// 箱型の当たり判定
/// </summary>
class BoxCollider : public Collider
{
	//Colliderクラスのprivateメンバにアクセスできるようにする
	friend class Collider;

public:

	/// <summary>
	/// コンストラクタ(当たり判定の作成)
	/// </summary>
	/// <param name="basePos">当たり判定の中心位置（ゲームオブジェクトの原点から見た位置)</param>
	/// <param name="size">	//引数：size	当たり判定のサイズ（幅、高さ、奥行き）</param>
	BoxCollider(XMFLOAT3 _basePos, XMFLOAT3 _size);

private:

	/// <summary>
	/// 接触判定
	/// </summary>
	/// <param name="target">相手の当たり判定</param>
	/// <returns>接触してればtrue</returns>
	bool IsHit(Collider* _target) override;
};

