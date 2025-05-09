#pragma once
#define _USE_MATH_DEFINES
#include <dxgidebug.h>
#include <cmath>
#include <vector>
#include <imgui.h>

#include "D3DResourceLeakChecker.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "PipelineManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "Logger.h"
#include "Camera.h"
#include "CameraManager.h"
#include "SrvManager.h"
#include "ImGuiManager.h"
#include "GlobalVariables.h"
#include "AudioManager.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#include "ParticleManager.h"
#include "LightManager.h"
#include "TimeManager.h"
#include "Line3D.h"

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

	//ポインタ
	//WindowsAPI
	std::unique_ptr<WinApp> winApp = nullptr;
	//DirecX
	std::unique_ptr<DirectXCommon> dxCommon = nullptr;
	//入力
	Input* input_ = nullptr;
	//SRV
	std::unique_ptr<SrvManager> srvManager = nullptr;
	//ImGuiマネージャ
	std::unique_ptr<ImGuiManager> imGuiManager = nullptr;
	//
	GlobalVariables* globalVariables_ = nullptr;
	//パイプラインマネージャ
	PipelineManager* pipelineManager = nullptr;
	//テクスチャマネージャ
	TextureManager* textureManager_ = nullptr;
	//パーティクルマネージャ
	ParticleManager* particleManager_ = nullptr;
	//モデルマネージャ
	ModelManager* modelManager_ = nullptr;
	//ラインマネージャ
	Line3dManager* lineManager_ = nullptr;
	//ライトマネージャ
	LightManager* lightManager_ = nullptr;
	//カメラマネージャ
	CameraManager* cameraManager_ = nullptr;
	//オーディオマネージャの初期化
	AudioManager* audioManager_ = nullptr;
	//シーン
	SceneManager* sceneManager_ = nullptr;
	//シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
	//タイムマネージャ
	TimeManager* timeManager_ = nullptr;
};

