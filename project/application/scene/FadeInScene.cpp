#include "FadeInScene.h"
#include "SceneManager.h"
#include "TimeManager.h"

void FadeInScene::Initialize(){
	BaseScene::Initialize();

	//スプライトの初期化
	for (uint32_t i = 0; i < 1; ++i) {
		std::unique_ptr<Sprite> sprite(new Sprite);
		sprite->Initialize("white.png");
		sprite->SetPosition({ 0,0 });
		sprite->SetSize({ 1280.0f,720.0f });
		sprite->SetColor({ 0.0f,0.0f,0.0f,1.0f });
		sprites_.push_back(std::move(sprite));
	}
}

void FadeInScene::Finalize(){
	//解放
	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite.reset();  // メモリを解放する
	}

	BaseScene::Finalize();
}

void FadeInScene::Update(){
	BaseScene::Update();
	counter_ += TimeManager::GetInstance()->deltaTime_;
	if (counter_ < duration_) {
		float alpha = 1.0f - (counter_ / duration_);
		sprites_[0]->SetColor({ 0.0f,0.0f,0.0f,alpha });
	} else {
		sprites_[0]->SetColor({ 0.0f,0.0f,0.0f,0.0f });
		sceneManager_->RemoveScene("FADE_IN");
	}

	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Update();
	}
}

void FadeInScene::Draw(){
	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Draw();
	}
}
