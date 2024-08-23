#pragma once
#include <string>
#include "ModelCommon.h"
#include "externals/DirectXTex/d3dx12.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class ModelCommon;
class Model{
private:
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct MaterialData {
		std::string mtlFilePath;
		std::string textureFilePath;
		uint32_t textureIndex = 0;
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
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	struct DirectionalLight {
		Vector4 color;//!<ライトの色
		Vector3 direction; //!< ライトの向き
		float intensity;//!< 輝度
	};
public://メンバ関数
	//初期化
	void Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& filename);
	//更新
	void Update();
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
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	//uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* wvpData = nullptr;
	DirectionalLight* directionalLightData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	//D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//Transform変数を作る。
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//カメラのTransform変数を作る。
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };
};

