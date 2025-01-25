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
		int isBlinnPhong; //!<BlinnPhongかどうか
	};
	struct PointLight{
		Vector4 color;//!<ライトの色
		Vector3 position; //!< ライトの場所
		float intensity;//!< 輝度
		float radius;//!<ライトの届く最大距離
		float decay;//!< 減衰率
		float padding[2];
	};
	struct SpotLight {
		Vector4 color;//!<ライトの色
		Vector3 position; //!< ライトの場所
		float intensity;//!< 輝度
		Vector3 direction; //!< ライトの向き
		float distance;//!<ライトの届く最大距離
		float decay;//!<減衰率
		float cosAngle;//!<スポットライトの余弦
		float cosFalloffStart;//!<Falloffの開始角度
		float padding[2];
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
	//PointLight
	ComPtr<ID3D12Resource> pointLightResource_;
	PointLight* pointLightData_ = nullptr;
	//SpotLight
	ComPtr<ID3D12Resource> spotLightResource_;
	SpotLight* spotLightData_;
public://ゲッターセッター
	DirectionalLight* GetDirectionalLight() const { return directionalLightData_; }
	PointLight* GetPointLight() const { return pointLightData_; }
	SpotLight* GetSpotLight() const { return spotLightData_; }

	void SetDirectionalLight(const DirectionalLight& directionalLight);
	void SetPointLight(const PointLight& pointLight);
	void SetSpotLight(const SpotLight& spotLight);
};