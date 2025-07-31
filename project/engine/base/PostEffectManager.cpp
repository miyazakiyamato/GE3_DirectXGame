#include "PostEffectManager.h"
#include "PipelineManager.h"
#include "CameraManager.h"
#include "TimeManager.h"
#include "TextureManager.h"
#include <imgui.h>

PostEffectManager* PostEffectManager::instance = nullptr;

PostEffectManager* PostEffectManager::GetInstance(){
	if (instance == nullptr) {
		instance = new PostEffectManager;
	}
	return instance;
}

void PostEffectManager::Initialize(DirectXCommon* dxCommon, SrvUavManager* srvUavManager){
	dxCommon_ = dxCommon;
	srvUavManager_ = srvUavManager;

	TextureManager::GetInstance()->LoadTexture("noise0.png"); // マスクテクスチャの読み込み

	// PostEffectManagerの初期化処理
	PipelineState pipelineState_{};
	pipelineState_.shaderName = "OffScreen/Fullscreen";
	pipelineState_.blendMode = BlendMode::kNone; // オフスクリーン描画はブレンドなし
	pipelineState_.cullMode = CullMode::kNone; // カリングなし
	pipelineState_.fillMode = FillMode::kSolid; // ソリッド描画
	pipelineState_.depthMode = DepthMode::kDisable; // 深度テストを無効にする
	pipelineState_.isOffScreen = true; // オフスクリーン描画フラグを設定
	offScreenName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState_);

	// ここでは、必要なリソースの作成や初期設定を行う
	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	// 初期値の設定
	material_->projectionInverseMatrix = Matrix4x4::MakeIdentity4x4();
	material_->depthSensitivity = 1.0f; // 深度感度の初期値
	material_->threshold = 0.5f; // マスクの閾値の初期値
	material_->edgeColor = Vector3(1.0f, 1.0f, 1.0f); // エッジの色の初期値
	material_->time = 0.0f; // 時間の初期値
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
	material_->time += TimeManager::GetInstance()->deltaTime_; // 時間の更新
	if (CameraManager::GetInstance()->GetCamera()) {
		// カメラの投影行列を取得
		const Matrix4x4& projectionMatrix = CameraManager::GetInstance()->GetCamera()->GetProjectionMatrix();
		// 逆行列を計算
		material_->projectionInverseMatrix = projectionMatrix.Inverse().Transpose();
	} else {
		// カメラが設定されていない場合は、単位行列を設定
		material_->projectionInverseMatrix = Matrix4x4::MakeIdentity4x4();
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
	if (postEffectName_ == "Dissolve") {
		srvUavManager_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvIndex(maskTextureName_));
	}
	if (postEffectName_ == "DepthBasedOutline") {
		dxCommon_->OffScreenDepthDraw();
	} else {
		dxCommon_->OffScreenDraw();
	}
}

void PostEffectManager::ImGuiUpdate() {
	std::string groupName = "PostEffect";
	if (ImGui::BeginMenu(groupName.c_str())) {
		//エフェクト選択
		std::vector<std::string> effectState{
				"Fullscreen", //!< フルスクリーンエフェクト
				"Grayscale",
				"Vignette", //!< ビネットエフェクト
				"BoxFilter", //!< ボックスフィルターエフェクト
				"GaussianFilter", //!< ガウシアンフィルターエフェクト
				"LuminanceBasedOutline", //!< 輝度ベースのアウトラインエフェクト
				"DepthBasedOutline", //!< 深度ベースのアウトラインエフェクト
				"RadialBlur", //!< ラジアルブラーエフェクト
				"Dissolve", //!< Dissolveエフェクト
				"Random" //!< ランダムエフェクト
		};
		std::string textureItem_selected_idx = postEffectName_;
		const char* currentItem = textureItem_selected_idx.c_str();
		if (ImGui::BeginCombo("now PostEffect", currentItem)) {
			for (int i = 0; i < effectState.size(); ++i) {
				bool isSelected = (textureItem_selected_idx == effectState[i]);
				if (ImGui::Selectable(effectState[i].c_str(), isSelected)) {
					AddPostEffect(effectState[i]);
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Text("PostEffect Name: %s", postEffectName_.c_str());
		ImGui::SliderFloat("Depth Sensitivity", &material_->depthSensitivity, 0.0f, 10000.0f);
		ImGui::SliderFloat("Threshold", &material_->threshold, 0.0f, 1.0f);
		ImGui::ColorEdit3("Edge Color", &material_->edgeColor.x);
		ImGui::Text("Time: %.2f", material_->time);
		ImGui::EndMenu();
	}
}

void PostEffectManager::AddPostEffect(const std::string& name){
	postEffectName_ = name;
	// PostEffectManagerの初期化処理
	PipelineState pipelineState_{};
	pipelineState_.shaderName = "OffScreen/" + name;
	pipelineState_.blendMode = BlendMode::kNone; // オフスクリーン描画はブレンドなし
	pipelineState_.cullMode = CullMode::kNone; // カリングなし
	pipelineState_.fillMode = FillMode::kSolid; // ソリッド描画
	pipelineState_.depthMode = DepthMode::kDisable; // 深度テストを無効にする
	pipelineState_.isOffScreen = true; // オフスクリーン描画フラグを設定
	offScreenName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState_);
}

void PostEffectManager::RemovePostEffect(const std::string& name){
	
}
