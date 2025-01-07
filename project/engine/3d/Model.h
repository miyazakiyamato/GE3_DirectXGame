#pragma once
#include "DirectXCommon.h"
#include "BlendMode.h"
#include "Matrix4x4.h"

class Model{
private:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//構造体
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct MaterialData {
		std::string mtlFilePath;
		std::string textureFilePath;
	};
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
	};
public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxCommon, const std::string& directoryPath, const std::string& filename);
	//描画
	void Draw();
	//.mtlファイルの読み取り
	void LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み取り
	void LoadObjFile(const std::string& directoryPath, const std::string& filename);
	//カラー
	Vector4 LoadColor();
private:
	DirectXCommon* dxCommon_ = nullptr;
	//Objファイルのデータ
	ModelData modelData;
	//バッファリソース
	ComPtr<ID3D12Resource> vertexResource;
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	//uint32_t* indexData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	//D3D12_INDEX_BUFFER_VIEW indexBufferView;
public://ゲッターセッター
};

