#include "CameraManager.h"

CameraManager* CameraManager::instance = nullptr;

CameraManager* CameraManager::GetInstance(){
	if (instance == nullptr) {
		instance = new CameraManager;
	}
	return instance;
}

void CameraManager::Initialize(){

}

void CameraManager::Finalize(){
	delete instance;
	instance = nullptr;
}

void CameraManager::SetCamera(const std::string& filePath){
	//読み込み済みモデルを検索
	if (cameras.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Camera> SettingCamera = std::make_unique<Camera>();

	//モデルをmapコンテナに格納する
	cameras.insert(std::make_pair(filePath, std::move(SettingCamera)));
}

void CameraManager::FindCamera(const std::string& filePath){
	//読み込み済みモデルを検索
	if (cameras.contains(filePath)) {
		//読み込みモデルを戻り値としてreturn
		camera_ =  cameras.at(filePath).get();
	}
}
