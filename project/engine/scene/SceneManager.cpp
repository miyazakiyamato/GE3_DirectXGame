#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::GetInstance(){
	//インスタンスがなければ生成
	if (instance == nullptr) {
		instance = new SceneManager;
	}
	return instance;
}

void SceneManager::Finalize(){
	//シーンの終了処理
	for (const auto& pair : scenes_) {
		pair.second->Finalize();
	}
	scenes_.clear();

	delete instance;
	instance = nullptr;
}

void SceneManager::Update(){
	//削除シーンの処理
	for (const std::string& sceneName : removeSceneNames_) {
		auto it = scenes_.find(sceneName);
		if (it != scenes_.end()) {
			it->second->Finalize();
			scenes_.erase(it);
		}
	}
	removeSceneNames_.clear();

	//追加シーンの処理
	for (const std::string& sceneName : addSceneNames_) {
		assert(sceneFactory_);
		if (scenes_.find(sceneName) == scenes_.end()) {
			scenes_[sceneName] = std::unique_ptr<BaseScene>(sceneFactory_->CreateScene(sceneName));
			assert(scenes_[sceneName]);
			scenes_[sceneName]->Initialize();
			scenes_[sceneName]->SetSceneManager(this);
		}
	}
	addSceneNames_.clear();

	//シーンの更新
	for(const auto& pair : scenes_){
		pair.second->Update();
	}
}

void SceneManager::Draw(){
	//シーンの描画
	for (const auto& pair : scenes_) {
		pair.second->Draw();
	}
}

void SceneManager::AddScene(const std::string& sceneName){
	addSceneNames_.push_back(sceneName);
}

void SceneManager::RemoveScene(const std::string& sceneName){
	removeSceneNames_.push_back(sceneName);
}

bool SceneManager::IsSceneAlive(const std::string& sceneName){
	auto it = scenes_.find(sceneName);
	return it != scenes_.end();
	return false;
}

bool SceneManager::IsSceneFinished(const std::string& sceneName){
	auto it = scenes_.find(sceneName);
	if (it != scenes_.end()) {
		return it->second->IsFinished();
	}
	return false;
}

BaseScene* SceneManager::GetScene(const std::string& sceneName) {
	auto it = scenes_.find(sceneName);
	if (it != scenes_.end()) {
		return it->second.get();
	}
	return nullptr;
}