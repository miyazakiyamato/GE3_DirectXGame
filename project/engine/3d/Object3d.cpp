#include "Object3d.h"
#include "ModelManager.h"
#include "CameraManager.h"

using namespace Microsoft::WRL;

void Object3d::Initialize(){
	modelCommon_ = ModelManager::GetInstance()->GetModelCommon();

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
}

void Object3d::Update(){
	//3D
	//transform.rotate.y += 0.03f;
	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjectionMatrix;
	if (CameraManager::GetInstance()->GetCamera()) {
		const Matrix4x4& viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
		worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
	}
	else {
		worldViewProjectionMatrix = worldMatrix;
	}
	wvpData->WVP = worldViewProjectionMatrix;
	wvpData->World = worldMatrix;
}

void Object3d::Draw(){
	// コマンドリストの取得
	ComPtr<ID3D12GraphicsCommandList> commandList = modelCommon_->GetDxCommon()->GetCommandList();

	//wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
	//Lighting
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.Get()->GetGPUVirtualAddress());

	if (model_) {
		model_->Draw();
	}

}

void Object3d::SetModel(const std::string& filePath){
	//モデルを検索してセット
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}
