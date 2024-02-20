#pragma once
//�C���N���[�h
#include <dInput.h>
#include <XInput.h>
#include <DirectXMath.h>
//�����J
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib,"Xinput.lib")
using namespace DirectX;

/// <summary>
/// DirectInput���g�����L�[�{�[�h���͏���
/// </summary>
namespace Input
{
	void Initialize(HWND _hWnd);
	void Update();
	void Release();
	//���L�[�{�[�h
	bool IsKey(int _keyCode);
	bool IsKeyDown(int _keyCode);
	bool IsKeyUp(int _keyCode);
	//���}�E�X
	bool IsMouseButton(int _buttonCode);
	bool IsMouseButtonDown(int _buttonCode);
	bool IsMouseButtonUp(int _buttonCode);
	XMFLOAT3 GetMousePosition();
	XMFLOAT3 GetMouseMove();
	void SetMousePosition(int _x, int _y);
	//���R���g���[���[
	bool IsPadButton(int _buttonCode, int _padID = 0);
	bool IsPadButtonDown(int _buttonCode, int _padID = 0);
	bool IsPadButtonUp(int _buttonCode, int _padID = 0);
	XMFLOAT3 GetPadStickL(int _padID = 0);
	XMFLOAT3 GetPadStickR(int _padId = 0);
	float GetPadTrrigerL(int _padID = 0);
	float GetPadTrrigerR(int _padID = 0);
	void SetPadVibration(int _l, int _r, int _padID = 0);
};
