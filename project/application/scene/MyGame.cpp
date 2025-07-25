#include "MyGame.h"

void MyGame::Initialize(){
	//基底クラスの初期化処理
	Framework::Initialize();

	//最初のシーンセット
	sceneManager_->ChangeScene("GAME");
	//オフスクリーンの描画設定
	//sceneManager_->ChangeOffScreenState("Fullscreen");
	//sceneManager_->ChangeOffScreenState("Grayscale");
	//sceneManager_->ChangeOffScreenState("Vignette");
	//sceneManager_->ChangeOffScreenState("BoxFilter");
	//sceneManager_->ChangeOffScreenState("GaussianFilter");
	//sceneManager_->ChangeOffScreenState("LuminanceBasedOutline");
	//sceneManager_->ChangeOffScreenState("DepthBasedOutline");
	//sceneManager_->ChangeOffScreenState("RadialBlur");
	sceneManager_->ChangeOffScreenState("Dissolve");
	textureManager_->LoadTexture("noise0.png");
	textureManager_->LoadTexture("noise1.png");
	//オフスクリーンの初期化
	offScreen_->SetDepthSensitivity(5000.0f);
}

void MyGame::Finalize(){
	//基底クラスの終了処理
	Framework::Finalize();
}

void MyGame::Update(){
	//基底クラスの更新処理
	Framework::Update();
	float threshold = offScreen_->GetThreshold();
	Vector3 edgeColor = offScreen_->GetEdgeColor();
	if (input_->PushKey(DIK_A)) {
		threshold -= 0.01f; // 閾値を減少
	}
	if (input_->PushKey(DIK_D)) {
		threshold += 0.01f; // 閾値を増加
	}
	if (input_->PushKey(DIK_Q)) {
		edgeColor.x -= 0.1f; // 赤成分を減少
	}
	if (input_->PushKey(DIK_E)) {
		edgeColor.x += 0.1f; // 赤成分を増加
	}
	threshold = std::clamp(threshold, 0.0f, 1.0f); // 閾値を0から1の範囲に制限
	offScreen_->SetThreshold(threshold);
	offScreen_->SetEdgeColor(edgeColor); // 赤色に設定
}

void MyGame::Draw(){
	offScreen_->Update();
	//描画前処理
	dxCommon->RenderTexturePreDraw();
	srvManager->PreDraw();
		
	sceneManager_->Draw();

	//OffScreenの描画
	dxCommon->SwapChainPreDraw();
	sceneManager_->OffScreenDrawSetting();
	offScreen_->Draw();
	srvManager->SetGraphicsRootDescriptorTable(2, textureManager_->GetSrvIndex("noise0.png"));
	dxCommon->OffScreenDraw();
#ifdef _DEBUG
	//実際のcommandListのImGuiの描画コマンドを積む
	imGuiManager->Draw();
#endif //_DEBUG

	//描画後処理
	dxCommon->PostDraw();
}
