#pragma once
#include <memory>
#include <map>
#include <d3d12.h>
#include <wrl.h>
#include "Matrix4x4.h"

class DirectXCommon;
class LightManager {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct DirectionalLight {
		Vector4 color;//!<ライトの色
		Vector3 direction; //!< ライトの向き
		float intensity;//!< 輝度
	};
public://メンバ関数
	//シングルトンインスタンスの取得
	static LightManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//描画
	void Draw();
	//終了
	void Finalize();

private://シングルインスタンス
	static LightManager* instance;

	LightManager() = default;
	~LightManager() = default;
	LightManager(LightManager&) = delete;
	LightManager& operator=(LightManager&) = delete;
private://メンバ変数
	//ポインタ
	DirectXCommon* dxCommon_ = nullptr;
	//ライトデータ
	//DirectionalLight
	ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLight* directionalLightData_ = nullptr;

public://ゲッターセッター
	DirectionalLight* GetDirectionalLight() const { return directionalLightData_; }

	void SetDirectionalLight(const DirectionalLight& directionalLight);
};