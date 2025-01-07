#include "TitleScene.h"
#include "SceneManager.h"
#include <TextureManager.h>
#include <AudioManager.h>

void TitleScene::Initialize(){
	BaseScene::Initialize();
	//スプライトの初期化
	for (uint32_t i = 0; i < 1; ++i) {
		Sprite* sprite = new Sprite;
		sprite->Initialize("resources/title.png");
		//sprite->SetPosition({ 100 + 200.0f * float(i), 100 });
		sprite->SetSize({ 1280.0f,720.0f });
		sprites.push_back(sprite);
	}

	
}

void TitleScene::Finalize(){
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	BaseScene::Finalize();
}

void TitleScene::Update(){
	BaseScene::Update();
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");
	}
	for (Sprite* sprite : sprites) {
		sprite->Update();
	}
	
}

void TitleScene::Draw(){
	//Spriteの描画準備Spriteの描画に共通のグラフィックコマンドを積む
	TextureManager::GetInstance()->DrawCommonSetting();
	for (Sprite* sprite : sprites) {
		sprite->Draw();
	}
}
