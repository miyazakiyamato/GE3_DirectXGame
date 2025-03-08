#include "Object3d.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "PipelineManager.h"

void Object3d::Initialize(){
	dxCommon_ = ModelManager::GetInstance()->GetDirectXCommon();
	lightManager_ = LightManager::GetInstance();
	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource = dxCommon_->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	wvpData->WVP = Matrix4x4::MakeIdentity4x4();
	wvpData->World = Matrix4x4::MakeIdentity4x4();
	wvpData->WorldInverseTranspose = Matrix4x4::MakeIdentity4x4();

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//カメラ用のリソースを作る。今回はcolor1つ分のサイズを用意する
	cameraResource = dxCommon_->CreateBufferResource(sizeof(CameraForGpu));
	//カメラのデータを書き込む
	//書き込むためのアドレスを取得
	cameraResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	cameraData->worldPosition = { 0.0f,0.0f,0.0f };
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

		Matrix4x4 cameraWorldMatrix = CameraManager::GetInstance()->GetCamera()->GetWorldMatrix();

		cameraData->worldPosition = {cameraWorldMatrix.m[3][0],cameraWorldMatrix.m[3][1],cameraWorldMatrix.m[3][2]};
	}
	else {
		worldViewProjectionMatrix = worldMatrix;
	}
	wvpData->WVP = worldViewProjectionMatrix;
	wvpData->World = worldMatrix;
	wvpData->WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(worldMatrix));
	if (model_) {
		wvpData->WVP = (Matrix4x4)model_->GetModelData().rootNode.localMatrix * worldViewProjectionMatrix;
		wvpData->World = (Matrix4x4)model_->GetModelData().rootNode.localMatrix * worldMatrix;
		wvpData->WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(wvpData->World));
	}
}

void Object3d::Draw(){

	if (model_) {
		// コマンドリストの取得
		ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

		//パイプラインを設定
		PipelineManager::GetInstance()->DrawSetting(PipelineState::kModel,blendMode_);
		//wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
		//カメラCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(4, cameraResource.Get()->GetGPUVirtualAddress());
		
		lightManager_->Draw();

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
	materialData->shininess = 40.0f;
	materialData->highLightColor = { 1.0f,1.0f,1.0f,1.0f };
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
