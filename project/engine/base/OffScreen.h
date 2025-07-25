#pragma once
#include "Matrix4x4.h"
#include "DirectXCommon.h"

class OffScreen{
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
	};
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//更新
	void Update();
	//描画
	void Draw();
	////ImGuiでの編集用
	//void ImGuiUpdate(const std::string& name);
private:
	DirectXCommon* dxCommon_ = nullptr;

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

