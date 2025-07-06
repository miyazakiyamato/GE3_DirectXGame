#include "CameraManager.h"
#include <imgui.h>
#include <GlobalVariables.h>

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

void CameraManager::ImGuiUpdate(){
	std::vector<std::string> keys;
	// std::transformを使用してキーを抽出
	std::transform(cameras.begin(), cameras.end(), std::back_inserter(keys),
		[](const auto& pair) { return pair.first; });
	std::string groupName = "Camera";
	if (ImGui::BeginMenu(groupName.c_str())) {
		std::string cameraItem_selected_idx = nowCameraName_;
		const char* currentItem = cameraItem_selected_idx.c_str();
		if (ImGui::BeginCombo("Now Camera", currentItem)) {
			for (int i = 0; i < keys.size(); ++i) {
				bool isSelected = (cameraItem_selected_idx == keys[i]);
				if (ImGui::Selectable(keys[i].c_str(), isSelected)) {
					nowCameraName_ = keys[i];
					camera_ = cameras[keys[i]].get();
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		Vector3 cameraRotate = camera_->GetRotate();
		ImGui::DragFloat3("Camera.Rotate", &cameraRotate.x, 0.001f);

		Vector3 cameraPosition = camera_->GetTranslate();
		ImGui::DragFloat3("Camera.Translate", &cameraPosition.x, 0.01f);

		camera_->SetRotate(cameraRotate);
		camera_->SetTranslate(cameraPosition);
		ImGui::EndMenu();
	}
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
