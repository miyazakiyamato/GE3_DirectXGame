#include "Offscreen.h"
#include "CameraManager.h"

void OffScreen::Initialize(DirectXCommon* dxCommon){
	dxCommon_ = dxCommon;
	// Offscreenの初期化処理
	// ここでは、必要なリソースの作成や初期設定を行う
	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&material));
	// 初期値の設定
	material->projectionInverseMatrix = Matrix4x4::MakeIdentity4x4();
	material->depthSensitivity = 1.0f; // 深度感度の初期値
}

void OffScreen::Update(){
	if (CameraManager::GetInstance()->GetCamera()) {
		// カメラの投影行列を取得
		const Matrix4x4& projectionMatrix = CameraManager::GetInstance()->GetCamera()->GetProjectionMatrix();
		// 逆行列を計算
		material->projectionInverseMatrix = projectionMatrix.Inverse().Transpose();
	} else {
		// カメラが設定されていない場合は、単位行列を設定
		material->projectionInverseMatrix = Matrix4x4::MakeIdentity4x4();
	}
}

void OffScreen::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, materialResource.Get()->GetGPUVirtualAddress());
}
