#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "Camera.h"
#include "LightManager.h"
#include "Animation.h"
#include "Skeleton.h"
#include "SkinCluster.h"

class Object3d{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
private:
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
	};
	struct Material {
		Vector4 color{1,1,1,1};
		Vector4 highLightColor{ 1,1,1,1 };
		Matrix4x4 uvTransform;
		int enableLighting; // ライティングを有効にするかどうか
		float shininess; // シェーダーの光沢度
		int enableEnvironmentMap; // 環境マップを有効にするかどうか
		float environmentCoefficient; // 環境マップの寄与度
	};
	struct CameraForGpu {
		Vector3 worldPosition;
		float padding;
	};
	struct AnimationData{
		Animation* animation;
		float time = 0.0f;
		bool isLoop = false;
	};

public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
private://メンバ変数
	DirectXCommon* dxCommon_ = nullptr;

	Model* model_ = nullptr;
	std::string textureFilePath_ = "";
	BlendMode blendMode_ = BlendMode::kNormal;
	std::string pipelineStateName_ = "";
	//バッファリソース
	ComPtr<ID3D12Resource> wvpResource;
	ComPtr<ID3D12Resource> materialResource;
	ComPtr<ID3D12Resource> cameraResource;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* wvpData = nullptr;
	Material* materialData = nullptr;
	CameraForGpu* cameraData = nullptr;
	LightManager* lightManager_ = nullptr;

	//Transform変数を作る。
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	std::unique_ptr<AnimationData> animationData_ = nullptr;
	std::unique_ptr<Skeleton> skeletonData_ = nullptr;
	std::unique_ptr<SkinCluster> skinClusterData_ = nullptr;

	Object3d* parent_ = nullptr;

	bool isDrawSkeleton_ = false; //!< Skeletonを描画するかどうか
	bool isSkybox_ = false; //!< SkyBoxかどうか
	std::string environmentTextureFilePath_ = "";
public://ゲッターセッター
	const BlendMode& GetBlendMode() { return blendMode_; }
	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }
	const Vector4& GetColor() const { return materialData->color; }
	const Vector4& GetHighLightColor() const { return materialData->highLightColor; }
	Vector3 GetCenterPosition() const;
	const Matrix4x4& GetWorldMatrix() const { return wvpData->World; }
	bool GetEnableLighting() const { return materialData->enableLighting; }
	float GetShininess() const { return materialData->shininess; }
	bool GetEnableEnvironmentMap() const { return materialData->enableEnvironmentMap; }
	float GetEnvironmentCoefficient() const { return materialData->environmentCoefficient; }

	void SetParent(Object3d* parent) { parent_ = parent; }
	void SetTexture(std::string textureFilePath) { textureFilePath_ = textureFilePath; }
	void SetEnvironmentTexture(const std::string& cubeTextureFilePath);
	void SetModel(const std::string& filePath);
	void SetAnimation(const std::string& filePath,bool isLoop);
	void SetBlendMode(BlendMode blendMode);
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetColor(const Vector4& color) { materialData->color = color; }
	void SetHighLightColor(const Vector4& color) { materialData->highLightColor = color; }
	void SetEnableLighting(bool enable) { materialData->enableLighting = enable; }
	void SetShininess(float shininess) { materialData->shininess = shininess; }
	void SetEnableEnvironmentMap(bool enable) { materialData->enableEnvironmentMap = enable; }
	void SetEnvironmentCoefficient(float coefficient) {materialData->environmentCoefficient = coefficient;}
	void SetDrawSkeleton(bool isDraw) { isDrawSkeleton_ = isDraw; }
};

