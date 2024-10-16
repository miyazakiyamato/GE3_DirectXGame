#pragma once
#include "Input.h"

class SceneManager;
class BaseScene{
public://メンバ関数
	//初期化
	virtual void Initialize();
	//終了
	virtual void Finalize();
	//毎フレーム更新
	virtual void Update();
	//描画
	virtual void Draw() = 0;
private://メンバ変数

protected://継承
	SceneManager* sceneManager_ = nullptr;
	Input* input_ = nullptr;
public://ゲッターセッター
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }
};

