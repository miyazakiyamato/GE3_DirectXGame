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

void CameraManager::SetCamera(const std::string& cameraName){
	//読み込み済みカメラを検索
	if (cameras.contains(cameraName)) {
		//読み込み済みなら早期return
		return;
	}
	//カメラの生成とファイル読み込み、初期化
	std::unique_ptr<Camera> SettingCamera = std::make_unique<Camera>();

	//カメラをmapコンテナに格納する
	cameras.insert(std::make_pair(cameraName, std::move(SettingCamera)));
}

void CameraManager::FindCamera(const std::string& cameraName){
	if (nowCameraName_ == cameraName) { return; }
	//読み込み済みカメラを検索
	if (cameras.contains(cameraName)) {
		//読み込みカメラを戻り値としてreturn
		camera_ =  cameras.at(cameraName).get();
		nowCameraName_ = cameraName;
	}
}
