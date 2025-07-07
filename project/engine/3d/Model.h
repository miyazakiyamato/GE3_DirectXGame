#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <optional>
#include <map>
#include "DirectXCommon.h"
#include "BlendMode.h"
#include "Quaternion.h"

class Model{
private:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//構造体
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct EulerTransform {
		Vector3 scale;
		Vector3 rotate;//Eulerでの回転
		Vector3 translate;
	};
	struct QuaternionTransform {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};
	struct Node{
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};
	struct MeshMaterialData {
		//EulerTransform uvTransform;
		Vector4 color;
		std::string textureFilePath;
	};
	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};
	struct JointWeightData {
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};
	struct Mesh {
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		MeshMaterialData material;
		std::map<std::string, JointWeightData> skinClusterData;
		//バッファリソース
		ComPtr<ID3D12Resource> vertexResource;
		ComPtr<ID3D12Resource> indexResource;
		//バッファリソース内のデータを指すポインタ
		VertexData* vertexData = nullptr;
		uint32_t* indexData = nullptr;
		//バッファリソースの使い道を補足するバッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
	};

public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxCommon, const std::string& directoryPath, const std::string& filename);
	//描画
	void Draw(size_t meshIndex);
	//ファイルの読み取り
	void LoadFile(const std::string& directoryPath, const std::string& filename);
	//Node解析
	Node ReadNode(aiNode* node);
	//カラー
	void LoadColor(Mesh& mesh,aiMaterial* material);
private:
	bool IsPrimitive(const std::string& filename);
	void ModelDataSkybox();
private://メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	//Objファイルのデータ
	std::vector<Mesh> meshDates_;
	size_t meshCount_ = 0;
	Node rootNode_;
public://ゲッターセッター
	const std::vector<Mesh>& GetMeshData() { return meshDates_; }
	const Node& GetNode() { return rootNode_; }
};

