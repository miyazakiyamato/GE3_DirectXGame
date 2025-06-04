#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new SceneManager;
	}
	return instance;
}

void SceneManager::Finalize(){
	scene_->Finalize();
	delete scene_;

	delete instance;
	instance = nullptr;
}

void SceneManager::Update(){
	//次のシーンの予約があるなら
	if (nextScene_) {
		//旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}
		//シーンの切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;
		//次のシーンを初期化する
		scene_->Initialize();
		scene_->SetSceneManager(this);
	}

	scene_->Update();
}

void SceneManager::Draw(){
	scene_->Draw();
}

void SceneManager::OffScreenDrawSetting(){
	PipelineManager::GetInstance()->DrawSetting(offScreenName_);
}

void SceneManager::ChangeScene(std::string SceneName){
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	nextScene_ = sceneFactory_->CreateScene(SceneName);
}

void SceneManager::ChangeOffScreenState(std::string shaderName){
	pipelineState_.shaderName = "OffScreen/" + shaderName;
	pipelineState_.blendMode = BlendMode::kNone; // オフスクリーン描画はブレンドなし
	pipelineState_.cullMode = CullMode::kNone; // カリングなし
	pipelineState_.fillMode = FillMode::kSolid; // ソリッド描画
	pipelineState_.depthMode = DepthMode::kDisable; // 深度テストを無効にする
	pipelineState_.isOffScreen = true; // オフスクリーン描画フラグを設定
	offScreenName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState_);
}