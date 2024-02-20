#pragma once
//インクルード
#include <dInput.h>
#include <XInput.h>
#include <DirectXMath.h>
//リンカ
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib,"Xinput.lib")
using namespace DirectX;

/// <summary>
/// DirectInputを使ったキーボード入力処理
/// </summary>
namespace Input
{
	void Initialize(HWND _hWnd);
	void Update();
	void Release();
	//▼キーボード
	bool IsKey(int _keyCode);
	bool IsKeyDown(int _keyCode);
	bool IsKeyUp(int _keyCode);
	//▼マウス
	bool IsMouseButton(int _buttonCode);
	bool IsMouseButtonDown(int _buttonCode);
	bool IsMouseButtonUp(int _buttonCode);
	XMFLOAT3 GetMousePosition();
	XMFLOAT3 GetMouseMove();
	void SetMousePosition(int _x, int _y);
	//▼コントローラー
	bool IsPadButton(int _buttonCode, int _padID = 0);
	bool IsPadButtonDown(int _buttonCode, int _padID = 0);
	bool IsPadButtonUp(int _buttonCode, int _padID = 0);
	XMFLOAT3 GetPadStickL(int _padID = 0);
	XMFLOAT3 GetPadStickR(int _padId = 0);
	float GetPadTrrigerL(int _padID = 0);
	float GetPadTrrigerR(int _padID = 0);
	void SetPadVibration(int _l, int _r, int _padID = 0);
};
