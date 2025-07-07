#include "Model.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include "TextureManager.h"
#include "Line3D.h"

void Model::Initialize(DirectXCommon* dxCommon, const std::string& directoryPath, const std::string& filename){
	dxCommon_ = dxCommon;
	if (!IsPrimitive(filename)) {
		//モデルの読み込み
		LoadFile(directoryPath, "model/" + filename);
	}
	for (Mesh& meshData : meshDates_) {
		//頂点リソースを作る
		meshData.vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * meshData.vertices.size());
		//頂点バッファビューを作成する
		//リソースの先頭のアドレスから使う
		meshData.vertexBufferView.BufferLocation = meshData.vertexResource.Get()->GetGPUVirtualAddress();
		//使用するリソースのサイズは頂点3つのサイズ
		meshData.vertexBufferView.SizeInBytes = sizeof(VertexData) * UINT(meshData.vertices.size());
		//1頂点当たりのサイズ
		meshData.vertexBufferView.StrideInBytes = sizeof(VertexData);

		//頂点リソースにデータを書き込む
		meshData.vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&meshData.vertexData));//書き込むためのアドレスを取得
		std::memcpy(meshData.vertexData, meshData.vertices.data(), sizeof(VertexData) * meshData.vertices.size());//頂点データをリソースにコピー

		//頂点リソースを作る
		meshData.indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * meshData.indices.size());
		//頂点バッファビューを作成する
		//リソースの先頭のアドレスから使う
		meshData.indexBufferView.BufferLocation = meshData.indexResource.Get()->GetGPUVirtualAddress();
		//使用するリソースのサイズは頂点3つのサイズ
		meshData.indexBufferView.SizeInBytes = sizeof(uint32_t) * UINT(meshData.indices.size());
		//インデックスはuint32_tとする
		meshData.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

		//頂点リソースにデータを書き込む
		meshData.indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&meshData.indexData));//書き込むためのアドレスを取得
		std::memcpy(meshData.indexData, meshData.indices.data(), sizeof(uint32_t) * meshData.indices.size());//頂点データをリソースにコピー

		//.objの参照しているテクスチャファイル読み込み
		if (meshData.material.textureFilePath.empty()) {
			//mtlファイルがない場合は、白いテクスチャを設定
			meshData.material.textureFilePath = "white.png";
		}
		//テクスチャマネージャーに読み込みを依頼
		TextureManager::GetInstance()->LoadTexture(meshData.material.textureFilePath);
	}
}

void Model::Draw(std::vector<MaterialData> materialDates){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	size_t meshCount = 0;
	for (Mesh& meshData : meshDates_) {
		commandList->IASetVertexBuffers(0, 1, &meshData.vertexBufferView); //VBVを設定
		commandList->IASetIndexBuffer(&meshData.indexBufferView);//IBVを設定

		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialDates[meshCount].materialResource.Get()->GetGPUVirtualAddress());
		//テクスチャを設定
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(materialDates[meshCount].textureFilePath_));
		//描画！(DrawCall/ドローコール)3頂点で1つのインスタンス。インスタンスについては
		commandList->DrawIndexedInstanced(UINT(meshData.indices.size()), 1, 0, 0, 0);
		meshCount++;
	}
}

void Model::LoadFile(const std::string& directoryPath, const std::string& filename) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがないのは対応しない
	meshCount_ = (size_t)scene->mNumMeshes;//メッシュの数を取得
	meshDates_.resize(meshCount_);//読み込み前に初期化
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));//TexcoordがないMeshは今回は非対応
		meshDates_[meshIndex].vertices.resize(mesh->mNumVertices);//頂点数のメモリだけ確保
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices;++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			meshDates_[meshIndex].vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			meshDates_[meshIndex].vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			meshDates_[meshIndex].vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみサポート

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				meshDates_[meshIndex].indices.push_back(vertexIndex);
			}
		}
		//SkinClusterの情報を取得
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			//Jointごとの格納領域をつくる
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = meshDates_[meshIndex].skinClusterData[jointName];
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

		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
			aiMaterial* material = scene->mMaterials[materialIndex];
			LoadColor(meshDates_[meshIndex],material);
		}
	}
	rootNode_ = ReadNode(scene->mRootNode);
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

void Model::LoadColor(Mesh& mesh,aiMaterial* material){
	
	//マテリアルの色を取得
	aiColor3D color(1.0f, 1.0f, 1.0f);
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
		mesh.material.color = { color.r,color.g,color.b,1.0f };
	} else {
		mesh.material.color = {1.0f,1.0f,1.0f,1.0f};//mtlファイルがある場合はmtlファイルから色を読み込む
	}
	if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
		aiString textureFilePath;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
		mesh.material.textureFilePath = textureFilePath.C_Str();
	}
}

bool Model::IsPrimitive(const std::string& filename){
	if (filename == "skybox") {
		ModelDataSkybox();
		return true;
	}
	return false;
}

void Model::ModelDataSkybox(){
	meshDates_.resize(1);
	Vector3 points[8]{};
	Vector3 min = { -1.0f,-1.0f,-1.0f };
	Vector3 max = { 1.0f,1.0f,1.0f };
	points[0] = max;
	points[1] = { max.x,max.y,min.z };
	points[2] = { max.x,min.y,max.z };
	points[3] = { max.x,min.y,min.z };

	points[4] = { min.x,max.y,min.z };
	points[5] = { min.x,max.y,max.z };
	points[6] = { min.x,min.y,max.z };
	points[7] = min;
	for (int32_t index = 0; index < 8; ++index) {
		meshDates_[0].vertices.push_back({
			{ points[index].x,points[index].y,points[index].z,1.0f }, // position
			{ 0.0f,0.0f }, // texcoord
			{ 0.0f,0.0f,-1.0f } // normal
			});
	}
	meshDates_[0].indices = {
		0, 1, 2, 2, 1, 3,
		4, 5, 7, 7, 5, 6,
		5, 0, 6, 6, 0, 2,
		1, 4, 3, 3, 4, 7,
		0, 5, 1, 1, 5, 4,
		2, 3, 6, 6, 3, 7,
	};
	meshDates_[0].material.color = { 1.0f,1.0f,1.0f,1.0f };
	rootNode_ = Node{
		{ { 1.0f,1.0f,1.0f },{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f}},
		Matrix4x4::MakeIdentity4x4(),
		"skybox",
		{}
	};
}