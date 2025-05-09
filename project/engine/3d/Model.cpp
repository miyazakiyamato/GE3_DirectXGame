#include "Model.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include "TextureManager.h"
#include "Line3D.h"

void Model::Initialize(DirectXCommon* dxCommon, const std::string& directoryPath, const std::string& filename){
	dxCommon_ = dxCommon;

	//モデルの読み込み
	LoadObjFile(directoryPath,"model/" + filename);
	//頂点リソースを作る
	vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
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

	//頂点リソースを作る
	indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * modelData.indices.size());
	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource.Get()->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つのサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * UINT(modelData.indices.size());
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//頂点リソースにデータを書き込む
	indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));//書き込むためのアドレスを取得
	std::memcpy(indexData, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());//頂点データをリソースにコピー

	//.objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
}

void Model::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
	//SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath));
	//描画！(DrawCall/ドローコール)3頂点で1つのインスタンス。インスタンスについては
	commandList->DrawIndexedInstanced(UINT(modelData.indices.size()), 1, 0, 0, 0);
}

void Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがないのは対応しない

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));//TexcoordがないMeshは今回は非対応
		modelData.vertices.resize(mesh->mNumVertices);//頂点数のメモリだけ確保
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices;++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			modelData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみサポート

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}
		//SkinClusterの情報を取得
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			//Jointごとの格納領域をつくる
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];
			//InverseBindPoseMatrixを格納
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);//assimpの行列からSTRを抽出する関数を利用
			Matrix4x4 bindPoseMatrix = Quaternion::MakeAffineMatrix(
				{ scale.x,scale.y,scale.z },
				{ rotate.x,-rotate.y,-rotate.z,rotate.w },
				{ -translate.x,translate.y,translate.z });//x軸を反転、回転軸が逆なので軸反転
			jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();
			//Weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}
	}

	modelData.rootNode = ReadNode(scene->mRootNode);

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = textureFilePath.C_Str();
		}
	}
}

Model::Node Model::ReadNode(aiNode* node){
	Node result;
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);//assimpの行列からSTRを抽出する関数を利用
	result.transform.scale = { scale.x,scale.y,scale.z };
	result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };//x軸を反転、回転軸が逆なので軸反転
	result.transform.translate = { -translate.x,translate.y,translate.z };//x軸を反転
	result.localMatrix = Quaternion::MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);

	//aiMatrix4x4 aiLocalMatrix = node->mTransformation;//nodeのlocakMatrixを取得
	//aiLocalMatrix.Transpose();//列ベクトル形式を行ベクトル形式に転換
	//for (int i = 0; i < 4; ++i) {
	//	for (int j = 0; j < 4; ++j){
	//		result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
	//	}
	//}
	result.name = node->mName.C_Str();//Node名を格納
	result.children.resize(node->mNumChildren);//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

Vector4 Model::LoadColor()
{
	Vector4 color = {1.0f,1.0f,1.0f,1.0f};
	//std::string line;//ファイルから読んだ1行を格納するもの
	//std::ifstream file(modelData.material.mtlFilePath);//ファイルを開く
	//assert(file.is_open());//開けなかったら止める
	////実際にファイルを読む
	//while (std::getline(file, line)) {
	//	std::string identifier;
	//	std::istringstream s(line);
	//	s >> identifier;//先頭の識別子を読む
	//	if (identifier == "Kd") {
	//		s >> color.x >> color.y >> color.z;
	//		color.w = 1.0f;
	//	}
	//}
	return color;
}