#pragma once
#include "BaseScene.h"

class SceneManager{
public://メンバ関数
	//シングルインスタンスの取得
	static SceneManager* GetInstance();
	//終了
	void Finalize();
	//毎フレーム更新
	void Update();
	//描画
	void Draw();
	//次のシーン予約
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }
private://シングルインスタンス
	static SceneManager* instance;

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;
private://メンバ変数
	//今のシーン
	BaseScene* scene_ = nullptr;
	//次のシーン
	BaseScene* nextScene_ = nullptr;
};

