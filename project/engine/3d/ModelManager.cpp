#include "ModelManager.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new ModelManager;
	}
	return instance;
}

void ModelManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager){
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;
}

void ModelManager::Finalize(){
	delete instance;
	instance = nullptr;
}

void ModelManager::LoadModel(const std::string& filePath){
	//読み込み済みモデルを検索
	if (models.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(dxCommon_, "resources/", filePath);

	//モデルをmapコンテナに格納する
	models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath){
	//読み込み済みモデルを検索
	if (models.contains(filePath)) {
		//読み込みモデルを戻り値としてreturn
		return models.at(filePath).get();
	}

	//ファイル名一致なし
	return nullptr;
}

void ModelManager::LoadAnimation(const std::string& filePath){
	//読み込み済みアニメーションを検索
	if (animations.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}
	//アニメーションの生成とファイル読み込み、初期化
	std::unique_ptr<Animation> animation = std::make_unique<Animation>();
	animation->LoadFile(filePath);

	//アニメーションをmapコンテナに格納する
	animations.insert(std::make_pair(filePath, std::move(animation)));
}

Animation* ModelManager::FindAnimation(const std::string& filePath){
	//読み込み済みアニメーションを検索
	if (animations.contains(filePath)) {
		//読み込みアニメーションを戻り値としてreturn
		return animations.at(filePath).get();
	}
	//ファイル名一致なし
	return nullptr;
}
