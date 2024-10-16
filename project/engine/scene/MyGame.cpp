#include "MyGame.h"
#include "GameScene.h"
#include "TitleScene.h"

void MyGame::Initialize(){
	//基底クラスの初期化処理
	Framework::Initialize();

	//最初のシーンセット
	sceneManager->SetNextScene(new TitleScene());
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

	//Modelの描画準備Modelの描画に共通グラフィックコマンドを積む
	ModelManager::GetInstance()->DrawCommonSetting();

	//Spriteの描画準備Spriteの描画に共通のグラフィックコマンドを積む
	TextureManager::GetInstance()->DrawCommonSetting();

	CameraManager::GetInstance()->GetCamera()->Update();
		
	sceneManager->Draw();

#ifdef _DEBUG
	//実際のcommandListのImGuiの描画コマンドを積む
	imGuiManager->Draw();
#endif //_DEBUG

	//描画後処理
	dxCommon->PostDraw();
}
