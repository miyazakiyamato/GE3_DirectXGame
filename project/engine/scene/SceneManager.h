#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include <PipelineManager.h>

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
	void OffScreenDrawSetting();
	//次のシーン予約
	void ChangeScene(std::string SceneName);
	//次のオフスクリーン
	void ChangeOffScreenState(std::string shaderName);
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
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
	//offScreenの描画
	std::string offScreenName_ = "";
	PipelineState pipelineState_{};
public://ゲッターセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }
};

