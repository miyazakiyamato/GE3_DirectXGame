#include "Line3D.h"
#include "PipelineManager.h"
#include "TextureManager.h"

void Line3D::Initialize() {

	//Line用の頂点リソースを作る
	vertexResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 2);
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 2;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//IndexLine用のインデックスリソースを作る
	indexResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 2);
	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource.Get()->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 2;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(Material));
	materialData = nullptr;//マテリアルにデータを書き込む
	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));//書き込むためのアドレスを取得
	//マテリアルデータの初期値を書き込む
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);//白を書き込む

	//Line用のTransformationMatirx用のリソースを作る。Marix4x4 1つ分のサイズを用意する
	wvpResource = PipelineManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
	wvpResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));//書き込むためのアドレスを取得
	//単位行列を書き込んどく
	wvpData->WVP = Matrix4x4::MakeIdentity4x4();
	wvpData->World = Matrix4x4::MakeIdentity4x4();

	//頂点リソースにデータを書き込む
	vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//頂点座標
	vertexData[0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[1].position = { 1.0f,0.0f,0.0f,1.0f };
	//
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };

	//インデックスリソースにデータを書き込む
	indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1;
}

void Line3D::Update() {

	//頂点座標
	vertexData[0].position = { line.origin.x,line.origin.y,line.origin.z,1.0f };//左下
	vertexData[1].position = { line.origin.x + line.diff.x, line.origin.y + line.diff.y, line.origin.z + line.diff.z,1.0f };//左上

	//Line用のWorldViewProjectionMtrixを作る
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, {}, line.origin);
	Matrix4x4 viewMatrix = Matrix4x4::MakeIdentity4x4();
	Matrix4x4 projectionMatrix = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	wvpData->WVP = worldViewProjectionMatrix;
	wvpData->World = worldMatrix;
}

void Line3D::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = PipelineManager::GetInstance()->GetDxCommon()->GetCommandList();

	//パイプラインを設定
	PipelineManager::GetInstance()->DrawSetting(PipelineState::kLine3D, blendMode_);
	//Spriteの描画。変更が必要なものだけ変更する
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
	commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
	//描画！(DrawColl/ドローコール)
	//commandList->DrawInstanced(6, 1, 0, 0);
	commandList->DrawIndexedInstanced(2, 1, 0, 0, 0);
}