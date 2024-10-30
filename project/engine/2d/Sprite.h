#pragma once
#include "d3dx12.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class SpriteCommon;
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
	SpriteCommon* spriteCommon_ = nullptr;
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
public://ゲッターセッター
	const Vector2& GetPosition() { return position_; }
	void SetPosition(const Vector2& position) { position_ = position; }
	float GetRotation() { return rotation_; }
	void SetRotation(float rotation) { rotation_ = rotation; }
	const Vector2& GetSize() { return size_; }
	void SetSize(const Vector2& size) { size_ = size; }
	const Vector4& GetColor() { return materialData->color; }
	void SetColor(const Vector4& color) { materialData->color = color; }
	void SetTexture(std::string textureFilePath);
	const Vector2& GetAnchorPoint() const { return anchorPoint_; }
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
	bool GetIsFlipX() { return isFlipX_; }
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
	bool GetIsFlipY() { return isFlipY_; }
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }
	const Vector2& GetTextureLeftTop() { return textureLeftTop_; }
	void SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; }
	const Vector2& GetTextureSize() { return textureSize_; }
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; }
};

