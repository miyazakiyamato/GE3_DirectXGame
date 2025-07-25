#include "PostEffectManager.h"
#include "PipelineManager.h"
#include "CameraManager.h"
#include "TimeManager.h"

PostEffectManager* PostEffectManager::instance = nullptr;

PostEffectManager* PostEffectManager::GetInstance(){
	if (instance == nullptr) {
		instance = new PostEffectManager;
	}
	return instance;
}

void PostEffectManager::Initialize(DirectXCommon* dxCommon){
	dxCommon_ = dxCommon;

	// PostEffectManagerの初期化処理
	PipelineState pipelineState_{};
	pipelineState_.shaderName = "OffScreen/Random";
	pipelineState_.blendMode = BlendMode::kNone; // オフスクリーン描画はブレンドなし
	pipelineState_.cullMode = CullMode::kNone; // カリングなし
	pipelineState_.fillMode = FillMode::kSolid; // ソリッド描画
	pipelineState_.depthMode = DepthMode::kDisable; // 深度テストを無効にする
	pipelineState_.isOffScreen = true; // オフスクリーン描画フラグを設定
	offScreenName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState_);

	// ここでは、必要なリソースの作成や初期設定を行う
	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&material));
	// 初期値の設定
	material->projectionInverseMatrix = Matrix4x4::MakeIdentity4x4();
	material->depthSensitivity = 1.0f; // 深度感度の初期値
	material->threshold = 0.5f; // マスクの閾値の初期値
	material->edgeColor = Vector3(1.0f, 1.0f, 1.0f); // エッジの色の初期値
	material->time = 0.0f; // 時間の初期値
}

void PostEffectManager::Finalize() {
	if (materialResource) {
		materialResource->Unmap(0, nullptr);
		materialResource.Reset();
	}
	delete instance;
	instance = nullptr;
}

void PostEffectManager::Update(){
	material->time += TimeManager::GetInstance()->deltaTime_; // 時間の更新
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

void PostEffectManager::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	dxCommon_->SwapChainPreDraw();
	// パイプラインの設定
	PipelineManager::GetInstance()->DrawSetting(offScreenName_);
	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, materialResource.Get()->GetGPUVirtualAddress());
	//srvManager->SetGraphicsRootDescriptorTable(2, textureManager_->GetSrvIndex("noise0.png"));
	dxCommon_->OffScreenDraw();
}

void PostEffectManager::ImGuiUpdate() {
	//std::string groupName = "PostEffect";
	
}

void PostEffectManager::AddPostEffect(const std::string& name){
	
}

void PostEffectManager::RemovePostEffect(const std::string& name){
	
}
