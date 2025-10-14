#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include <PipelineManager.h>
#include <unordered_map>
#include <memory>

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
	//シーン追加
	void AddScene(const std::string& sceneName);
	//シーン削除
	void RemoveScene(const std::string& sceneName);
	//シーン生存確認
	bool IsSceneAlive(const std::string& sceneName);
	//シーン終了確認
	bool IsSceneFinished(const std::string& sceneName);
private://シングルインスタンス
	static SceneManager* instance;

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;
private://メンバ変数
	std::unordered_map<std::string, std::unique_ptr<BaseScene>> scenes_;
	std::list<std::string> addSceneNames_;
	std::list<std::string> removeSceneNames_;
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
public://ゲッターセッター
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }
	BaseScene* GetScene(const std::string& sceneName);
};

