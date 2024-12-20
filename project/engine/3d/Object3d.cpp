#include "Object3d.h"
#include "ModelManager.h"
#include "CameraManager.h"

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

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//DirectionalLightのリソースを作る。
	directionalLightResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
	//デフォルト値を書き込んでおく
	directionalLightResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
}

void Object3d::Update(){
	Matrix4x4 worldMatrix;
	Matrix4x4 worldViewProjectionMatrix;
	worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	if (parent_) {
		worldMatrix = worldMatrix * parent_->GetWorldMatrix();
	}
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

	if (model_) {
		// コマンドリストの取得
		ID3D12GraphicsCommandList* commandList = modelCommon_->GetDxCommon()->GetCommandList();

		//wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
		//Lighting
		commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.Get()->GetGPUVirtualAddress());
		model_->Draw();
	}

}

void Object3d::SetModel(const std::string& filePath){
	//モデルを検索してセット
	model_ = ModelManager::GetInstance()->FindModel(filePath);
	//マテリアルデータの初期値を書き込む
	materialData->color = model_->LoadColor();//色を書き込む
	materialData->enableLighting = true;//Lightingを有効にする
	materialData->uvTransform = Matrix4x4::MakeIdentity4x4();//UVTransform単位行列で初期化
}

Vector3 Object3d::GetCenterPosition() const
{
	Vector3 worldPos;
	//ワールド行列の平行移動成分取得
	worldPos.x = wvpData->World.m[3][0];
	worldPos.y = wvpData->World.m[3][1];
	worldPos.z = wvpData->World.m[3][2];

	return worldPos;
}
