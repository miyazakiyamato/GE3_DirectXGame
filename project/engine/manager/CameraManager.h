#pragma once
#include <memory>
#include <map>
#include "Camera.h"

class CameraManager{
public:
	//シングルトンインスタンスの取得
	static CameraManager* GetInstance();
	//初期化
	void Initialize();
	//終了
	void Finalize();

	//カメラの生成
	void SetCamera(const std::string& filePath);
	//カメラの検索
	void FindCamera(const std::string& filePath);
private:
	static CameraManager* instance;

	CameraManager() = default;
	~CameraManager() = default;
	CameraManager(CameraManager&) = delete;
	CameraManager& operator=(CameraManager&) = delete;
private:
	//カメラデータ
	std::map<std::string, std::unique_ptr<Camera>> cameras;

	Camera* camera_ = nullptr;
public:
	Camera* GetCamera() const { return camera_; }
};

