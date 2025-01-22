#pragma once
#include <map>
#include "Model.h"

class ModelManager{
public://メンバ関数
	//シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//終了
	void Finalize();

	//モデルファイルの読み込み
	void LoadModel(const std::string& filePath);
	//モデルの検索
	Model* FindModel(const std::string& filePath);
private://シングルインスタンス
	static ModelManager* instance;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;
private://メンバ変数
	//モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;
	
	DirectXCommon* dxCommon_ = nullptr;
public://ゲッターセッター
	DirectXCommon* GetDirectXCommon() { return dxCommon_; }
};

