#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp{
public://静的メンバ関数
	//ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//終了
	void Finalize();

	//メッセージの処理
	bool ProcessMessage();
public://定数
	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;
private://メンバ変数
	//ウインドウクラスの設定
	WNDCLASS wc{};
	//ウインドウハンドル
	HWND hwnd = nullptr;
public://getter
	HINSTANCE GetHInstance() const { return wc.hInstance; }
	HWND GetHwnd() const { return hwnd; }
};

