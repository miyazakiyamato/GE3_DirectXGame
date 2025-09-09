#pragma once

class WinApp;
class DirectXCommon;
class SrvUavManager;
class ImGuiManager{
public://メンバ関数
	//初期化
	void Initialize(WinApp* winApp,DirectXCommon* dxCommon,SrvUavManager* srvUavManager);
	//終了
	void Finalize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();
	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();
	/// <summary>
	/// 画面への描画
	/// </summary>
	void Draw();
private://メンバ変数
	//WindowsAPI
	WinApp* winApp_ = nullptr;
	//DirectXCommon
	DirectXCommon* dxCommon_ = nullptr;
	//SRV
	SrvUavManager* srvUavManager_ = nullptr;
};

