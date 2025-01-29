#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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
	struct Node
	{
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};
	struct MaterialData {
		std::string mtlFilePath;
		std::string textureFilePath;
	};
	struct ModelData {
		std::vector<VertexData> vertices;
		MaterialData material;
		Node rootNode;
	};
public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxCommon, const std::string& directoryPath, const std::string& filename);
	//描画
	void Draw();
	//.objファイルの読み取り
	void LoadObjFile(const std::string& directoryPath, const std::string& filename);
	//Node解析
	Node ReadNode(aiNode* node);
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
	const ModelData& GetModelData() { return modelData; }
};

