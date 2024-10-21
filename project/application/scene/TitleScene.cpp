#include "TitleScene.h"
#include "SceneManager.h"

void TitleScene::Initialize(){
	BaseScene::Initialize();

}

void TitleScene::Finalize(){
	BaseScene::Finalize();
}

void TitleScene::Update(){
	BaseScene::Update();
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");
	}
}

void TitleScene::Draw(){
	
}
