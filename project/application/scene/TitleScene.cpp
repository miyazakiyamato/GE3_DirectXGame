#include "TitleScene.h"
#include "SceneManager.h"
#include <PostEffectManager.h>
#include <GlobalVariables.h>
#include <imgui.h>
#include <CameraManager.h>
#include <LightManager.h>

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
#ifdef _DEBUG
	//// ウインドウフラグに NoResize を指定
	//ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoResize);
	//ImGui::ShowDemoWindow();
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	globalVariables->Update();
	std::string groupName = "";
	if (ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar()) {
			input_->ImGuiUpdate();
			CameraManager::GetInstance()->ImGuiUpdate();

			LightManager::GetInstance()->ImGuiUpdate();

			/*size_t object3dCount = 0;
			for (std::unique_ptr<Object3d>& object3d : object3ds_) {
				std::string objectName = ("Object3d" + std::to_string(object3dCount)).c_str();
				object3d->ImGuiUpdate(objectName);

				object3dCount++;
			}
			particleSystem_->UpdateGlobalVariables();

			groupName = "Sprite";
			uint32_t spriteIDIndex = 0;
			for (std::unique_ptr<Sprite>& sprite : sprites_) {
				std::string spriteName = ("Sprite" + std::to_string(spriteIDIndex)).c_str();
				sprite->ImGuiUpdate(spriteName);
				++spriteIDIndex;
			}*/
			PostEffectManager::GetInstance()->ImGuiUpdate();
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
#endif //_DEBUG
	CameraManager::GetInstance()->GetCamera()->Update();
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

	PostEffectManager::GetInstance()->Update();
}

void TitleScene::Draw(){
	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Draw();
	}
}
