#include "MyGame.h"

void MyGame::Initialize(){
	//基底クラスの初期化処理
	Framework::Initialize();

	//最初のシーンセット
	sceneManager_->ChangeScene("GAME");
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
	dxCommon->PreDraw();
	srvManager->PreDraw();

	CameraManager::GetInstance()->GetCamera()->Update();
		
	sceneManager_->Draw();

#ifdef _DEBUG
	//実際のcommandListのImGuiの描画コマンドを積む
	imGuiManager->Draw();
#endif //_DEBUG

	//描画後処理
	dxCommon->PostDraw();
}
