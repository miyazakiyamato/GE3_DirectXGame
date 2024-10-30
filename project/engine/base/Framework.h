#pragma once
#define _USE_MATH_DEFINES
#include <dxgidebug.h>
#include <cmath>
#include <vector>
#include <imgui.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "SpriteCommon.h"
#include "ModelManager.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"
#include "Camera.h"
#include "CameraManager.h"
#include "SrvManager.h"
#include "ImGuiManager.h"
#include "AudioManager.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#include "ParticleManager.h"

class Framework{
public://メンバ関数
	virtual ~Framework() = default;
	//初期化
	virtual void Initialize();
	//終了処理
	virtual void Finalize();
	//毎フレーム更新
	virtual void Update();
	//描画
	virtual void Draw() = 0;
	//実行
	void Run();
protected://メンバ変数
	//リークチェック
	D3DResourceLeakChecker leakCheck;

	//ポインタ
	//WindowsAPI
	WinApp* winApp = nullptr;
	//DirecX
	DirectXCommon* dxCommon = nullptr;
	//SRV
	SrvManager* srvManager = nullptr;
	//ImGui
	ImGuiManager* imGuiManager = nullptr;
	//シーン
	SceneManager* sceneManager_ = nullptr;
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
};

