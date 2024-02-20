#pragma once
//インクルード
#include <DirectXMath.h>
#include "Direct3D.h"
#include "GameObject.h"

using namespace DirectX;

/// <summary>
/// カメラ
/// </summary>
class Camera : public GameObject
{
private:
	XMFLOAT4 lightPos_;
	ID3D11Buffer* pCB_;
public:
	//初期化（プロジェクション行列作成）
	void Initialize() override;

	//更新（ビュー行列作成）
	void Update(int _type);

	void Update() override;

	void Draw() override;
	
	void Release() override;

	void IntConstantBuffer(int _type);

	//▼セッター・ゲッター
	void SetPosition(XMVECTOR _position,int _type);
	void SetPosition(XMFLOAT3 _position,int _type);
	void SetTarget(XMVECTOR _target,int _type);
	void SetTarget(XMFLOAT3 _target,int _type);
	XMVECTOR GetPosition(int _type);
	XMVECTOR GetTarget(int _type);
	XMFLOAT4 GetLightPos() { return(lightPos_); }
	XMMATRIX GetViewMatrix();											//ビュー行列を取得
	XMMATRIX GetProjectionMatrix();										//プロジェクション行列を取得
};