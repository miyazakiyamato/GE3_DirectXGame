#include "TitleScene.h"
#include "SceneManager.h"

void TitleScene::Initialize(){
	BaseScene::Initialize();

	//スプライトの初期化
	for (uint32_t i = 0; i < 5; ++i) {
		std::unique_ptr<Sprite> sprite(new Sprite);
		sprite->Initialize("uvChecker.png");
		sprite->SetPosition({ 100 + 200.0f * float(i), 100 });
		sprite->SetSize({ 100.0f,100.0f });
		sprites_.push_back(std::move(sprite));
	}
}

void TitleScene::Finalize(){
	//解放
	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite.reset();  // メモリを解放する
	}

	BaseScene::Finalize();
}

void TitleScene::Update(){
	BaseScene::Update();
	if (input_->TriggerKey(DIK_SPACE) && sceneManager_->IsSceneAlive("FADE_OUT") == false) {
		sceneManager_->AddScene("FADE_OUT");
	}
	if (sceneManager_->IsSceneFinished("FADE_OUT")) {
		sceneManager_->RemoveScene("TITLE");
		sceneManager_->RemoveScene("FADE_OUT");
		sceneManager_->AddScene("GAME");
		sceneManager_->AddScene("FADE_IN");
	}
	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Update();
	}
}

void TitleScene::Draw(){
	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Draw();
	}
}
