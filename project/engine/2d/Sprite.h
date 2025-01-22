#pragma once
#include "d3dx12.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "BlendMode.h"

class Sprite{
private:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//構造体
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
	void Initialize(std::string textureFilePath);
	//更新
	void Update();
	//描画
	void Draw();
private://ローカル関数
	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();
private://メンバ変数
	//バッファリソース
	ComPtr<ID3D12Resource> vertexResource;
	ComPtr<ID3D12Resource> indexResource;
	ComPtr<ID3D12Resource> materialResource;
	ComPtr<ID3D12Resource> wvpResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* wvpData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//テクスチャ
	std::string textureFilePath_ = "";
	//アンカーポイント
	Vector2 anchorPoint_ = { 0.0f,0.0f };
	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;

	Vector2 position_ = { 0.0f,0.0f };
	float rotation_ = 0.0f;
	Vector2 size_ = { 640.0f,360.0f };
	//テクスチャサイズ
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	Vector2 textureSize_ = { 100.0f,100.0f };

	BlendMode blendMode_ = BlendMode::kNormal;

public://ゲッターセッター
	const BlendMode& GetBlendMode() { return blendMode_; }
	const Vector2& GetPosition() { return position_; }
	float GetRotation() { return rotation_; }
	const Vector2& GetSize() { return size_; }
	const Vector4& GetColor() { return materialData->color; }
	const Vector2& GetAnchorPoint() const { return anchorPoint_; }
	bool GetIsFlipX() { return isFlipX_; }
	bool GetIsFlipY() { return isFlipY_; }
	const Vector2& GetTextureLeftTop() { return textureLeftTop_; }
	const Vector2& GetTextureSize() { return textureSize_; }
	
	void SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }
	void SetPosition(const Vector2& position) { position_ = position; }
	void SetRotation(float rotation) { rotation_ = rotation; }
	void SetSize(const Vector2& size) { size_ = size; }
	void SetColor(const Vector4& color) { materialData->color = color; }
	void SetTexture(std::string textureFilePath);
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }
	void SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; }
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; }
};

