#include "Input.h"
#include "../Global.h"

namespace Input
{
	HWND hWnd_;								//ウィンドウハンドル
	LPDIRECTINPUT8 pDInput_;				//DirectInputオブジェクト
	//▼キーボード
	LPDIRECTINPUTDEVICE8 pKeyDevice_;		//キーボードオブジェクト
	BYTE KeyState_[256];					//現在の各キーの状態
	BYTE prevKeyState_[256];				//前フレームでの各キーの状態
	//▼マウス
	LPDIRECTINPUTDEVICE8 pMouseDevice_;		//マウスオブジェクト
	DIMOUSESTATE mouseState_;				//マウスの状態
	DIMOUSESTATE prevMouseState_;			//前フレームのマウスの状態
	POINT mousePosition_;				//マウスカーソルの位置
	//▼コントローラー
	const int MAX_PAD_NUM = 4;
	XINPUT_STATE controllerState_[MAX_PAD_NUM];
	XINPUT_STATE prevControllerState_[MAX_PAD_NUM];

	void Initialize(HWND _hWnd)
	{
		hWnd_ = _hWnd;

		//▼DirectInput本体
		DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, 
			(VOID**)&pDInput_, nullptr);

		//▼キーボード
		pDInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr);
		pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);
		pKeyDevice_->SetCooperativeLevel(NULL,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

		//▼マウス
		pDInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr);
		pMouseDevice_->SetDataFormat(&c_dfDIMouse);
		pMouseDevice_->SetCooperativeLevel(hWnd_, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	}

	void Update()
	{
		//▼キーボード
		pKeyDevice_->Acquire();
		memcpy(prevKeyState_, KeyState_, sizeof(prevKeyState_));
		pKeyDevice_->GetDeviceState(sizeof(KeyState_), &KeyState_);
		//▼マウス
		pMouseDevice_->Acquire();
		memcpy(&prevMouseState_, &mouseState_, sizeof(mouseState_));
		pMouseDevice_->GetDeviceState(sizeof(mouseState_), &mouseState_);
		//▼コントローラー
		for (int i = 0; i < MAX_PAD_NUM; i++)
		{
			memcpy(&prevControllerState_[i], &controllerState_[i], sizeof(controllerState_[i]));
			XInputGetState(i, &controllerState_[i]);
		}
	}

	void Release()
	{
		SAFE_RELEASE(pMouseDevice_);
		SAFE_RELEASE(pKeyDevice_);
		SAFE_RELEASE(pDInput_);
	}

	//▼キーボード
	bool IsKey(int _keyCode)
	{
		if (KeyState_[_keyCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	bool IsKeyDown(int _keyCode)
	{
		if (IsKey(_keyCode) && !(prevKeyState_[_keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsKeyUp(int _keyCode)
	{
		if (!IsKey(_keyCode) && (prevKeyState_[_keyCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	//▼マウス
	bool IsMouseButton(int _buttonCode)
	{
		if (mouseState_.rgbButtons[_buttonCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	bool IsMouseButtonDown(int _buttonCode)
	{
		if (IsMouseButton(_buttonCode) && !(prevMouseState_.rgbButtons[_buttonCode] & 0x80))
		{
			return true;
		}
		return false;
	}

	bool IsMouseButtonUp(int _buttonCode)
	{
		if (!IsMouseButton(_buttonCode) && prevMouseState_.rgbButtons[_buttonCode] & 0x80)
		{
			return true;
		}
		return false;
	}

	XMFLOAT3 GetMousePosition()
	{
		XMFLOAT3 result = XMFLOAT3((float)mousePosition_.x, (float)mousePosition_.y, 0);
		return result;
	}

	XMFLOAT3 GetMouseMove()
	{
		XMFLOAT3 result = XMFLOAT3((float)mouseState_.lX, (float)mouseState_.lY, (float)mouseState_.lZ);
		return result;
	}

	void SetMousePosition(int _x, int _y)
	{
		mousePosition_.x = _x;
		mousePosition_.y = _y;
	}

	//▼コントローラー
	bool IsPadButton(int _buttonCode, int _padID)
	{
		if (controllerState_[_padID].Gamepad.wButtons & _buttonCode)
		{
			return true;
		}
		return false;
	}

	bool IsPadButtonDown(int _buttonCode, int _padID)
	{
		if (IsPadButton(_buttonCode, _padID) && !(prevControllerState_[_padID].Gamepad.wButtons & _buttonCode))
		{
			return true;
		}
		return false;
	}

	bool IsPadButtonUp(int  _buttonCode, int _padID)
	{
		if (!IsPadButton(_buttonCode, _padID) && prevControllerState_[_padID].Gamepad.wButtons & _buttonCode)
		{
			return true;
		}
		return false;
	}

	float GetAnalogValue(int _raw, int _max, int _deadZone)
	{
		float result = (float)_raw;

		if (result > 0)
		{
			if (result < _deadZone)
			{
				result = 0;
			}
			else
			{
				result = (result - _deadZone) / (_max - _deadZone);
			}
		}

		else
		{
			if (result > -_deadZone)
			{
				result = 0;
			}
			else
			{
				result = (result + _deadZone) / (_max - _deadZone);
			}
		}

		return result;
	}

	XMFLOAT3 GetPadStickL(int _padID)
	{
		float x = GetAnalogValue(controllerState_[_padID].Gamepad.sThumbLX, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		float y = GetAnalogValue(controllerState_[_padID].Gamepad.sThumbLY, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		return XMFLOAT3(x, y, 0);
	}

	XMFLOAT3 GetPadStrickR(int _padID)
	{
		float x = GetAnalogValue(controllerState_[_padID].Gamepad.sThumbRX, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		float y = GetAnalogValue(controllerState_[_padID].Gamepad.sThumbRY, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		return XMFLOAT3(x, y, 0);
	}

	float GetPadTrrigerL(int _padID)
	{
		return GetAnalogValue(controllerState_[_padID].Gamepad.bLeftTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	float GetPadTrrigerR(int _padID)
	{
		return GetAnalogValue(controllerState_[_padID].Gamepad.bRightTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	void SetPadVibration(int _l, int _r, int _padID)
	{
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
		vibration.wLeftMotorSpeed = _l;
		vibration.wRightMotorSpeed = _r;
		XInputSetState(_padID, &vibration);
	}
}