#include "Particle.h"
#include "ParticleManager.h"
#include "CameraManager.h"
#include "TextureManager.h"

void Particle::Initialize(std::string textureFilePath) {
	particleCommon_ = ParticleManager::GetInstance()->GetParticleCommon();

	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	textureFilePath_ = textureFilePath;

	//Sprite用の頂点リソースを作る
	vertexResource = particleCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//IndexSprite用のインデックスリソースを作る
	indexResource = particleCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);

	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource.Get()->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = particleCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//マテリアルにデータを書き込む
	materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//マテリアルデータの初期値を書き込む
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);//白を書き込む
	materialData->enableLighting = false;//Lightingを無効にする
	materialData->uvTransform = Matrix4x4::MakeIdentity4x4();//UVTransform単位行列で初期化

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	instancingResource = particleCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix) * kNumInstance);
	//データを書き込む
	//書き込むためのアドレスを取得
	instancingResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNumInstance;++index) {
		instancingData[index].WVP = Matrix4x4::MakeIdentity4x4();
		instancingData[index].World = Matrix4x4::MakeIdentity4x4();
	}
	for (uint32_t index = 0; index < kNumInstance; ++index) {
		transforms[index].scale = { 1.0f,1.0f,1.0f };
		transforms[index].translate = { index * 0.1f,index * 0.1f,index * 0.1f };
	}
	//画像サイズをテクスチャサイズに合わせる
	AdjustTextureSize();
}

void Particle::Update() {
	//3D
	// //頂点リソースにデータを書き込む
	vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//三角形の頂点
	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;
	//左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	//上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}
	//頂点座標
	vertexData[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexData[1].position = { left,top,0.0f,1.0f };//左上
	vertexData[2].position = { right,bottom,0.0f,1.0f };//右下
	vertexData[3].position = { right,top,0.0f,1.0f };//右上

	//テクスチャの頂点
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath_);
	float tex_left = textureLeftTop_.x / metadata.width;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / metadata.width;
	float tex_top = textureLeftTop_.y / metadata.height;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metadata.height;

	//テクスチャ頂点座標
	vertexData[0].texcoord = { tex_left,tex_bottom };//左下
	vertexData[1].texcoord = { tex_left,tex_top };//左上
	vertexData[2].texcoord = { tex_right,tex_bottom };//右下
	vertexData[3].texcoord = { tex_right,tex_top };//右上

	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };
	vertexData[3].normal = { 0.0f,0.0f,-1.0f };
	//インデックスリソースにデータを書き込む
	indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

	//SpriteのUVTransform変数を作る。
	Transform uvTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	for (uint32_t index = 0; index < kNumInstance; ++index) {
		Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transforms[index].scale, transforms[index].rotate, transforms[index].translate);
		Matrix4x4 worldViewProjectionMatrix;
		if (CameraManager::GetInstance()->GetCamera()) {
			const Matrix4x4& viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
			worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
		}
		else {
			worldViewProjectionMatrix = worldMatrix;
		}
		instancingData[index].WVP = worldViewProjectionMatrix;
		instancingData[index].World = worldMatrix;
	}
	//SpriteのuvTransformを作る
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeAffineMatrix(uvTransform.scale, { 0.0f,0.0f,uvTransform.rotate.z }, uvTransform.translate);
	materialData->uvTransform = uvTransformMatrix;
}

void Particle::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = particleCommon_->GetDxCommon()->GetCommandList();
	/*for (uint32_t index = 0; index < kNumInstance; ++index) {

		
	}*/
	// Spriteの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, instancingResource.Get()->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath_));
	//描画！(DrawColl/ドローコール)
	//commandList->DrawInstanced(6, 1, 0, 0);
	commandList->DrawIndexedInstanced(6, kNumInstance, 0, 0, 0);
}

void Particle::AdjustTextureSize(){
	//テクスチャデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath_);
	//画像サイズをテクスチャサイズに合わせる
	textureSize_.x = static_cast<float>(metadata.width);
	textureSize_.y = static_cast<float>(metadata.height);
}
