#pragma once
#include <memory>
#include <unordered_map>
#include "Camera.h"

class CameraManager{
public:
	//シングルトンインスタンスの取得
	static CameraManager* GetInstance();
	//初期化
	void Initialize();
	//終了
	void Finalize();
	//ImGuiでの編集用
	void ImGuiUpdate();
	//カメラの生成
	void SetCamera(const std::string& cameraName);
	//カメラの検索
	void FindCamera(const std::string& cameraName);
private:
	static CameraManager* instance;

	CameraManager() = default;
	~CameraManager() = default;
	CameraManager(CameraManager&) = delete;
	CameraManager& operator=(CameraManager&) = delete;
private:
	//カメラデータ
	std::unordered_map<std::string, std::unique_ptr<Camera>> cameras;

	std::string nowCameraName_ = "";
	Camera* camera_ = nullptr;
public:
	Camera* GetCamera() const { return camera_; }
};

