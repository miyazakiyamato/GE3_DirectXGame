#pragma once
#include "d3dx12.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class ParticleCommon;
class Particle {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

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
	ParticleCommon* particleCommon_ = nullptr;

	
	//インスタンス数
	static const uint32_t kNumInstance = 10;
	//バッファリソース
	ComPtr<ID3D12Resource> vertexResource;
	ComPtr<ID3D12Resource> indexResource;
	ComPtr<ID3D12Resource> materialResource;
	ComPtr<ID3D12Resource> instancingResource;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* instancingData = nullptr;

	//Transform変数を作る。
	Transform transforms[kNumInstance]{};
	//テクスチャ
	std::string textureFilePath_ = "";
	//アンカーポイント
	Vector2 anchorPoint_ = { 0.0f,0.0f };
	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;
	//テクスチャサイズ
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	Vector2 textureSize_ = { 100.0f,100.0f };
public://ゲッターセッター
	
};

