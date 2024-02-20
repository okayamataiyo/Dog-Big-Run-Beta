#pragma once
//インクルード
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

//クラスの前方宣言
class GameObject;
class BoxCollider;
class SphereCollider;


//あたり判定のタイプ
enum ColliderType
{
	COLLIDER_BOX,		//箱型
	COLLIDER_CIRCLE		//球体
};

/// <summary>
/// 当たり判定を管理するクラス
/// </summary>
class Collider
{
	//それぞれのクラスのprivateメンバにアクセスできるようにする
	friend class BoxCollider;
	friend class SphereCollider;

protected:
	GameObject* pGameObject_;		//この判定をつけたゲームオブジェクト
	ColliderType	type_;			//種類
	XMFLOAT3		center_;		//中心位置（ゲームオブジェクトの原点から見た位置）
	XMFLOAT3		size_;			//判定サイズ（幅、高さ、奥行き）
	int				hDebugModel_;	//デバッグ表示用のモデルのID

public:
	//コンストラクタ
	Collider();

	//デストラクタ
	virtual ~Collider();

	/// <summary>
	/// 接触判定(継承先のSphereColliderかBoxColliderでオーバーライド)
	/// </summary>
	/// <param name="_target">相手の当たり判定</param>
	/// <returns>接触していればtrue</returns>
	virtual bool IsHit(Collider* _target) = 0;

	/// <summary>
	/// 箱型同士の衝突判定
	/// </summary>
	/// <param name="_boxA">1つ目の箱型判定</param>
	/// <param name="_boxB">2つ目の箱型判定</param>
	/// <returns>接触していればtrue</returns>
	bool IsHitBoxVsBox(BoxCollider* _boxA, BoxCollider* _boxB);

	/// <summary>
	/// 箱型と球体の衝突判定
	/// </summary>
	/// <param name="_box">1つ目の箱型判定</param>
	/// <param name="_sphere">2つ目の球体判定</param>
	/// <returns>接触していればtrue</returns>
	bool IsHitBoxVsCircle(BoxCollider* _box, SphereCollider* _sphere);

	/// <summary>
	/// 球体動詞の衝突判定
	/// </summary>
	/// <param name="_circleA">1つ目の球体判定</param>
	/// <param name="_circleB">2つ目の球体判定</param>
	/// <returns>接触していればtrue</returns>
	bool IsHitCircleVsCircle(SphereCollider* _circleA, SphereCollider* _circleB);

	/// <summary>
	/// テスト表示用の枠を描画
	/// </summary>
	/// <param name="_position">オブジェクトの位置</param>
	void Draw(XMFLOAT3 _position);

	//▼セッター・ゲッター
	void SetGameObject(GameObject* _gameObject) { pGameObject_ = _gameObject; }

};

