#pragma once
#include "Model.h"

class Object3d{
private:
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	struct DirectionalLight {
		Vector4 color;//!<ライトの色
		Vector3 direction; //!< ライトの向き
		float intensity;//!< 輝度
	};
public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//更新
	void Update();
	//描画
	void Draw();
private:
	DirectXCommon* dxCommon_ = nullptr;

	Model* model_ = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* wvpData = nullptr;
	DirectionalLight* directionalLightData = nullptr;

	//Transform変数を作る。
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//カメラのTransform変数を作る。
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };
public:
	void SetModel(Model* model) { model_ = model; }
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }

	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }
};

