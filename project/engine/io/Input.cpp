#include "Input.h"
#include "WinApp.h"
#include <cassert>

Input* Input::instance = nullptr;

Input* Input::GetInstance()
{
	if (instance == nullptr) {
		instance = new Input();
	}
	return instance;
}

void Input::Initialize(WinApp* winApp){
	//借りてきたWinAppのインスタンスを記録
	winApp_ = winApp;

	HRESULT result;
	//DirectInputの初期化
	directInput = nullptr;
	result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//キーボードデバイスの生成
	keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));
	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Finalize(){
	delete instance;
	instance = nullptr;
}

void Input::Update(){
	//前回のキー入力を保存先
	memcpy(keyPre, key, sizeof(key));
	//キーボード情報の取得開始
	keyboard->Acquire();
	//全キーの入力情報を取得する
	keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber){
	//指定キーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber){
	//指定キーを押していればtrueを返す
	if (!keyPre[keyNumber] && key[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}
