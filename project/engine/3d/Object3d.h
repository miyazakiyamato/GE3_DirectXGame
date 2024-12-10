#pragma once
#include "ModelCommon.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"

class Object3d{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
private:
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	struct Material {
		Vector4 color{1,1,1,1};
		int enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	struct DirectionalLight {
		Vector4 color;//!<ライトの色
		Vector3 direction; //!< ライトの向き
		float intensity;//!< 輝度
	};
	struct CameraForGpu {
		Vector3 worldPosition;
	};
public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
private://メンバ変数
	ModelCommon* modelCommon_ = nullptr;

	Model* model_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource> wvpResource;
	ComPtr<ID3D12Resource> materialResource;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* wvpData = nullptr;
	Material* materialData = nullptr;

	Light * directionalLight_ = nullptr;

	//Transform変数を作る。
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Object3d* parent_ = nullptr;

public://ゲッターセッター
	void SetModel(const std::string& filePath);
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetColor(const Vector4& color) { materialData->color = color; }
	void SetParent(Object3d* parent) { parent_ = parent; }
	void SetLight(Light* light) { directionalLight_ = light; }

	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }
	const Vector4& GetColor() const { return materialData->color; }
	Vector3 GetCenterPosition() const;
	const Matrix4x4& GetWorldMatrix() const { return wvpData->World; }
};

