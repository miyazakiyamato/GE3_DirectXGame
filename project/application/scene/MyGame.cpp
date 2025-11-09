#include "MyGame.h"

void MyGame::Initialize(){
	//基底クラスの初期化処理
	Framework::Initialize();

	//パイプラインを設定
	PipelineState pipelineState;
	pipelineState.shaderName = "Object3d";
	pipelineState.blendMode = BlendMode::kNormal;
	pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);

	//最初のシーンセット
	sceneManager_->AddScene("D_PARTICLE");
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
	srvUavManager->PreDraw();
	PipelineManager::GetInstance()->DrawSetting(pipelineStateName_);

	sceneManager_->Draw();

	//OffScreenの描画
	postEffectManager_->Draw();
#ifdef _DEBUG
	//実際のcommandListのImGuiの描画コマンドを積む
	imGuiManager->Draw();
#endif //_DEBUG

	//描画後処理
	dxCommon->PostDraw();
}
