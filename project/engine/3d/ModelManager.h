#pragma once
#include <map>
#include "Model.h"
#include "Animation.h"

class SrvManager;
class ModelManager{
public://メンバ関数
	//シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
	//終了
	void Finalize();

	//モデルファイルの読み込み
	void LoadModel(const std::string& filePath);
	//モデルの検索
	Model* FindModel(const std::string& filePath);

	//アニメーションファイルの読み込み
	void LoadAnimation(const std::string& filePath);
	//アニメーションの検索
	Animation* FindAnimation(const std::string& filePath);

private://シングルインスタンス
	static ModelManager* instance;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;
private://メンバ変数
	//ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	//モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;
	//アニメーションデータ
	std::map<std::string, std::unique_ptr<Animation>> animations;
public://ゲッターセッター
	DirectXCommon* GetDirectXCommon() { return dxCommon_; }
	SrvManager* GetSrvManager() { return srvManager_; }
};

