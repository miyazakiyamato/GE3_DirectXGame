#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "DirectXCommon.h"
#include "Matrix4x4.h"

class PostEffectManager{
private:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	struct Material {
		Matrix4x4 projectionInverseMatrix;
		float depthSensitivity = 1.0f;
		float threshold; // マスクの閾値
		float padding[2]; // 16バイト境界に合わせるためのパディング
		Vector3 edgeColor; // エッジの色
		float time = 0.0f; // 時間の変数（必要に応じて使用）
	};
public:
	//インスタンス取得
	static PostEffectManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//終了処理
	void Finalize();
	//更新
	void Update();
	//描画
	void Draw();
	//ImGuiでの編集用
	void ImGuiUpdate();
	//PostEffectの追加
	void AddPostEffect(const std::string& name);
	//PostEffectの削除
	void RemovePostEffect(const std::string& name);
private://シングルインスタンス
	static PostEffectManager* instance;

	PostEffectManager() = default;
	~PostEffectManager() = default;
	PostEffectManager(PostEffectManager&) = delete;
	PostEffectManager& operator=(PostEffectManager&) = delete;
private:
	DirectXCommon* dxCommon_ = nullptr;

	std::string offScreenName_; //!< オフスクリーンのパイプライン名
	ComPtr<ID3D12Resource> materialResource;
	Material* material = nullptr;
public:
	void SetDepthSensitivity(float sensitivity) { material->depthSensitivity = sensitivity; }
	void SetThreshold(float threshold) { material->threshold = threshold; }
	void SetEdgeColor(const Vector3& color) { material->edgeColor = color; }

	float GetDepthSensitivity() const { return material->depthSensitivity; }
	float GetThreshold() const { return material->threshold; }
	Vector3 GetEdgeColor() const { return material->edgeColor; }
};

