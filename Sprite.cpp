#include "Sprite.h"
#include "SpriteCommon.h"

using namespace Microsoft::WRL;

void Sprite::Initialize(SpriteCommon* spriteCommon){
	spriteCommon_ = spriteCommon;

	//Sprite用の頂点リソースを作る
	vertexResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 6);
	
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	
	//IndexSprite用のインデックスリソースを作る
	indexResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);
	
	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource.Get()->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//マテリアルにデータを書き込む
	materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//マテリアルデータの初期値を書き込む
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);//白を書き込む
	materialData->enableLighting = false;//Lightingを無効にする
	materialData->uvTransform = Matrix4x4::MakeIdentity4x4();//UVTransform単位行列で初期化

	//Sprite用のTransformationMatirx用のリソースを作る。Marix4x4 1つ分のサイズを用意する
	transformationMatrixResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	
	//書き込むためのアドレスを取得
	transformationMatrixResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	//単位行列を書き込んどく
	transformationMatrixData->WVP = Matrix4x4::MakeIdentity4x4();
	transformationMatrixData->World = Matrix4x4::MakeIdentity4x4();
}

void Sprite::Update(const Transform& transform,const Transform& uvTransform){
	//頂点リソースにデータを書き込む
	vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//三角形の頂点
	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };
	vertexData[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexData[3].texcoord = { 1.0f,0.0f };
	vertexData[3].normal = { 0.0f,0.0f,-1.0f };

	//インデックスリソースにデータを書き込む
	indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

	//Sprite用のWorldViewProjectionMtrixを作る
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 projectionMatrix = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;

	//SpriteのuvTransformを作る
	Matrix4x4 uvTransformMatrix = Matrix4x4::MakeAffineMatrix(uvTransform.scale, { 0.0f,0.0f,uvTransform.rotate.z }, uvTransform.translate);
	materialData->uvTransform = uvTransformMatrix;
}

void Sprite::Draw(){
	// コマンドリストの取得
	ComPtr<ID3D12GraphicsCommandList> commandList = spriteCommon_->GetDxCommon()->GetCommandList();

	//Spriteの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource.Get()->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
	commandList->SetGraphicsRootDescriptorTable(2, spriteCommon_->GetDxCommon()->GetSRVGPUDescriptorHandle(1));
	//描画！(DrawColl/ドローコール)
	//commandList->DrawInstanced(6, 1, 0, 0);
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
