#pragma once
#include "Matrix4x4.h"
#include <d3d12.h>
#include <wrl.h>

class DirectXCommon;
class Light{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct DirectionalLight {
		Vector4 color;//!<ライトの色
		Vector3 direction; //!< ライトの向き
		float intensity;//!< 輝度
	};
public:
	Light(){}
	~Light(){}
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//描画
	void Draw();
private:
	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12Resource> directionalLightResource;
	DirectionalLight* directionalLightData = nullptr;

public:
	const Vector4& GetColor() { return directionalLightData->color; }
	const Vector3& GetDirection() { return directionalLightData->direction; }
	const float& GetIntensity() { return directionalLightData->intensity; }

	void SetColor(const Vector4& color) { directionalLightData->color = color; }
	void SetDirection(const Vector3& direction) { directionalLightData->direction = direction; }
	void SetIntensity(const float& intensity) { directionalLightData->intensity = intensity; }
};

