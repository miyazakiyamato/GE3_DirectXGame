#pragma once
#include "d3dx12.h"
#include "Quaternion.h"
#include "BlendMode.h"

class Line3D{
private:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//構造体
	struct VertexData {
		Vector4 position;
	};
	struct Material {
		Vector4 color{1.0f,1.0f,1.0f,1.0f};
	};
	struct TransformationMatrix {
		Matrix4x4 WVP;
	};

	struct Line{
		Vector3 origin{0.0f,0.0f,0.0f};
		Vector3 diff{ 1.0f,1.0f,1.0f };
	};

public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();

	
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

	Line line{};

	BlendMode blendMode_ = BlendMode::kNormal;

public://ゲッターセッター
	//const BlendMode& GetBlendMode() { return blendMode_; }

	//void SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }
};

