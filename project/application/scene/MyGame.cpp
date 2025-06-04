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
	sceneManager_->ChangeOffScreenState("GaussianFilter");
}

void MyGame::Finalize(){
	//基底クラスの終了処理
	Framework::Finalize();
}

void MyGame::Update(){
	//基底クラスの更新処理
	Framework::Update();
}

void MyGame::Draw(){
	//描画前処理
	dxCommon->RenderTexturePreDraw();
	srvManager->PreDraw();
		
	sceneManager_->Draw();

	//OffScreenの描画
	dxCommon->SwapChainPreDraw();
	sceneManager_->OffScreenDrawSetting();
	dxCommon->OffScreenDraw();
#ifdef _DEBUG
	//実際のcommandListのImGuiの描画コマンドを積む
	imGuiManager->Draw();
#endif //_DEBUG

	//描画後処理
	dxCommon->PostDraw();
}
