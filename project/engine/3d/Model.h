#pragma once
#include <string>
#include "d3dx12.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class ModelCommon;
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
	struct Material {
		Vector4 color;
		int enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
public://メンバ関数
	//初期化
	void Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& filename);
	//描画
	void Draw();
	//.mtlファイルの読み取り
	void LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み取り
	void LoadObjFile(const std::string& directoryPath, const std::string& filename);
	//カラー
	void LoadColor(const std::string& filePath);
private:
	ModelCommon* modelCommon_ = nullptr;
	//Objファイルのデータ
	ModelData modelData;
	//バッファリソース
	ComPtr<ID3D12Resource> vertexResource;
	//ComPtr<ID3D12Resource> indexResource;
	ComPtr<ID3D12Resource> materialResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	//uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	//D3D12_INDEX_BUFFER_VIEW indexBufferView;
public://ゲッターセッター
	void SetColor(const Vector4& color) { materialData->color = color; }
	const Vector4& GetColor() { return materialData->color; }
};

