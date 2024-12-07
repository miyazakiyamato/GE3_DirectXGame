#include "ModelManager.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new ModelManager;
	}
	return instance;
}

void ModelManager::Initialize(DirectXCommon* dxCommon){
	modelCommon = std::make_unique<ModelCommon>();
	modelCommon->Initialize(dxCommon);
}

void ModelManager::Finalize(){
	delete instance;
	instance = nullptr;
}

void ModelManager::DrawCommonSetting(){
	modelCommon->DrawCommonSetting();
}

void ModelManager::ChangeBlendMode(ModelCommon::BlendMode blendMode){
	if (modelCommon->GetBlendMode() == blendMode) {
		return;
	}
	
	modelCommon->SetBlendMode(blendMode);
	modelCommon->CreateGraphicsPipeline();
}

void ModelManager::LoadModel(const std::string& filePath){
	//読み込み済みモデルを検索
	if (models.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelCommon.get(), "resources", filePath);

	//モデルをmapコンテナに格納する
	models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	//読み込み済みモデルを検索
	if (models.contains(filePath)) {
		//読み込みモデルを戻り値としてreturn
		return models.at(filePath).get();
	}

	//ファイル名一致なし
	return nullptr;
}
