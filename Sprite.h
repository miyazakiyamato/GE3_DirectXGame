#pragma once
#include "externals/DirectXTex/d3dx12.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class SpriteCommon;
class Sprite{
private:
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct Material {
		Vector4 color;
		int enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
public://メンバ関数
	//初期化
	void Initialize(SpriteCommon* spriteCommon);
	//更新
	void Update();
	//描画
	void Draw();
private:
	SpriteCommon* spriteCommon_ = nullptr;
	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	Vector2 position_ = { 0.0f,0.0f };
	float rotation_ = 0.0f;
	Vector2 size_ = { 640.0f,360.0f };
public:
	const Vector2& GetPosition() { return position_; }
	void SetPosition(const Vector2& position) { position_ = position; }
	float GetRotation() { return rotation_; }
	void SetRotation(float rotation) { rotation_ = rotation; }
	const Vector2& GetSize() { return size_; }
	void SetSize(const Vector2& size) { size_ = size; }
	const Vector4& GetColor() { return materialData->color; }
	void SetColor(const Vector4& color) { materialData->color = color; }
};

