#pragma once
#define DIRECTINPUT_VERSION 0x800
#include <Windows.h>
#include <dinput.h>
#include <wrl.h>
#include "WinApp.h"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class Input{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name = "keyNumber>キー番号(DIK_0等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);
	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name = "keyNumber>キー番号(DIK_0等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);
private:
	//WindowsAPI
	WinApp* winApp_ = nullptr;

	//DirectInput
	ComPtr<IDirectInput8> directInput;
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard;
	//全キーの状態
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE keyPre[256] = {};
};

