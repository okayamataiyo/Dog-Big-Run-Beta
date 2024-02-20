#pragma once
//インクルード
#include <d3d11.h>
#include <DirectXMath.h>
//リンカ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define SAFE_DELETE(p) if(p != nullptr){ delete p; p = nullptr;}
#define SAFE_RELEASE(p) if(p != nullptr){ p->Release(); p = nullptr;}

using namespace DirectX;


enum SHADER_TYPE
{
	SHADER_2D,
	SHADER_3D,
	SHADER_TOON,
	SHADER_TOONOUTLINE,
	SHADER_UNLIT,
	SHADER_MAX,
};

namespace Direct3D
{

	extern ID3D11Device* pDevice_;		                //デバイス
	extern ID3D11DeviceContext* pContext_;		        //デバイスコンテキスト
	extern bool isDrawCollision_;						//コリジョンを表示するかフラッグ
	extern int isChangeView_;							//ゲーム開始・終了の時に変化するフラグ
	extern float vPSize_[2];							//ビューポートのサイズ変更メンバ変数
	extern float prevVP_;								//ビューポートの元のサイズ
	extern float vPMove_[2];
	extern int widthHaif_;
	extern int width_;
	extern int height_;
	//初期化
	HRESULT Initialize(int winW, int winH, HWND hWnd);

	//シェーダー準備
	HRESULT InitShader();
	HRESULT InitShader3D();
	HRESULT InitShader2D();
	HRESULT InitShaderToon();
	HRESULT InitShaderToonOutLine();
	HRESULT InitShaderUNLIT();

	void SetShader(SHADER_TYPE type);

	void Update();

	//描画開始
	void BeginDraw();

	//描画終了
	void EndDraw();

	//解放
	void Release();

	/// <summary>
	/// 三角形と線分(レイ)の衝突判定(衝突判定に使用)
	/// </summary>
	/// <param name="_start">レイのスタート位置</param>
	/// <param name="_direction">レイの方向</param>
	/// <param name="_v0">三角形の各頂点位置</param>
	/// <param name="_v1">三角形の各頂点位置</param>
	/// <param name="_v2">三角形の各頂点位置</param>
	/// <param name="_distance">衝突点までの距離を返す</param>
	/// <returns>衝突したかどうか</returns>
	bool Intersect(XMFLOAT3& _start, XMFLOAT3& _direction,XMFLOAT3& _v0,XMFLOAT3& _v1, XMFLOAT3& _v2, float* _distance);

	/// <summary>
	/// ビューポートに情報を入れる
	/// </summary>
	/// <param name="_VpType">ビューポートの種類</param>
	void SetViewPort(int _VpType);

	void SetIsChangeView(int _IsChangeView);

};

