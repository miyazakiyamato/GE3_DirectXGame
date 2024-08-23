#include "Model.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include "TextureManager.h"

using namespace Microsoft::WRL;

void Model::Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& filename){
	modelCommon_ = modelCommon;

	//モデルの読み込み
	LoadObjFile(directoryPath,filename);
	//頂点リソースを作る
	vertexResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つのサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * UINT(modelData.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());//頂点データをリソースにコピー


	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//マテリアルデータの初期値を書き込む
	LoadColor(modelData.material.mtlFilePath);//色を書き込む
	materialData->enableLighting = true;//Lightingを有効にする
	materialData->uvTransform = Matrix4x4::MakeIdentity4x4();//UVTransform単位行列で初期化

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	wvpData->WVP = Matrix4x4::MakeIdentity4x4();
	wvpData->World = Matrix4x4::MakeIdentity4x4();


	//DirectionalLightのリソースを作る。
	directionalLightResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
	//デフォルト値を書き込んでおく
	directionalLightResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;

	//.objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	//読み込んだテクスチャの番号を取得
	modelData.material.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath);
}

void Model::Update(){
	//3D
	//transform.rotate.y += 0.03f;
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = Matrix4x4::MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Matrix4x4::Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = Matrix4x4::MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	wvpData->WVP = worldViewProjectionMatrix;
	wvpData->World = worldMatrix;
}

void Model::Draw(){
	// コマンドリストの取得
	ComPtr<ID3D12GraphicsCommandList> commandList = modelCommon_->GetDxCommon()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定
	//commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureIndex));
	//Lighting
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.Get()->GetGPUVirtualAddress());
	//描画！(DrawCall/ドローコール)3頂点で1つのインスタンス。インスタンスについては
	commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	//commandList->DrawIndexedInstanced(kIndexNum, 1, 0, 0, 0);
}

void Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//中で必要な関数の宣言
	std::string line;//ファイルから読んんだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//開けなかったら止める
	modelData.material.mtlFilePath = directoryPath + "/" + filename;
	//実際にファイルを読み、MaterialDataを構築する
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			modelData.material.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
}

void Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	//中で必要となる変数の宣言
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの
	//ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//開けなかったら止める
	//実際にファイルを読み、ModelDataを構築する
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcood;
			s >> texcood.x >> texcood.y;
			texcoords.push_back(texcood);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			//面は三角限定。その他は未対応
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				position.x *= -1.0f;
				normal.x *= -1.0f;
				texcoord.y = 1.0f - texcoord.y;
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
}

void Model::LoadColor(const std::string& filePath) {
	Vector4 color = {};
	std::string line;//ファイルから読んだ1行を格納するもの
	std::ifstream file(filePath);//ファイルを開く
	assert(file.is_open());//開けなかったら止める
	//実際にファイルを読む
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む
		if (identifier == "Kd") {
			s >> color.x >> color.y >> color.z >> color.w;
		}
	}
	materialData->color = color;
}

