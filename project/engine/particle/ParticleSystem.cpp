#include "ParticleSystem.h"
#include <imGui.h>
#include "ParticleManager.h"
#include "EmitterSphere.h"
#include "GlobalVariables.h"

void ParticleSystem::Finalize() {
	// すべてのエミッターを削除
	for (auto& emitter : emitters_) {
		emitter.second.reset();
	}
	emitters_.clear();
}

void ParticleSystem::Initialize(){
	globalVariables_ = GlobalVariables::GetInstance();
	const char* groupName = "ParticleSystem";

	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	/*for (const auto& [name, emitter] : emitters_) {
		emitter->ApplyGlobalVariables();
		emitter->InitializeGlobalVariables();
	}*/
}

void ParticleSystem::Update(){
	for (const auto& [name, emitter] : emitters_) {
		emitter->ApplyGlobalVariables();
		emitter->Update();
	}
	ParticleManager::GetInstance()->Update();
}

void ParticleSystem::Draw(){
	ParticleManager::GetInstance()->Draw();
}

void ParticleSystem::SetParticleEmitter(std::unique_ptr<BaseParticleEmitter> emitter){
	emitter->InitializeGlobalVariables();
	emitter->ApplyGlobalVariables();
	emitters_[emitter->GetName()] = std::move(emitter);
}

BaseParticleEmitter* ParticleSystem::GetParticleEmitter(const std::string& emitterName){
	if (emitters_.find(emitterName) != emitters_.end()) {
		return emitters_[emitterName].get();
	}
	return nullptr;
}

void ParticleSystem::Emit(const std::string& emitterName){
	emitters_[emitterName]->Emit();
}

void ParticleSystem::ImGuiUpdate(){
	if (ImGui::BeginMenu("ParticleSystem")) {
		// テキスト入力ボックス
		if (ImGui::InputText("Input EmitterNameText", textBuffer_, IM_ARRAYSIZE(textBuffer_))) {
			// 入力が変更された場合に反映
			emitterNameText_ = textBuffer_;
		}

		bool isGroupCreate = false;
		// 入力された文字列を表示
		ImGui::Text("Reflected Text: %s", emitterNameText_.c_str());
		if (emitterNameText_.empty()) {
			ImGui::Text("Error: Group name cannot be empty.");
		} else if (emitters_.count(emitterNameText_) != 0) {
			ImGui::Text("Error: Group name already exists.");
		} else {
			isGroupCreate = true;
		}

		// エミッタータイプ選択
		std::string typeSelect = "Create ParticleType Select";
		std::vector<std::string> emitterType{
			"Sphere",
		};
		const char* currentItem = nowEmitterTypeName_.c_str();
		if (ImGui::BeginCombo((typeSelect).c_str(), currentItem)) {
			for (int i = 0; i < emitterType.size(); ++i) {
				bool isSelected = (nowEmitterTypeName_ == emitterType[i]);
				if (ImGui::Selectable(emitterType[i].c_str(), isSelected)) {
					nowEmitterTypeName_ = emitterType[i];
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		// 作成するパーティクル最大数の設定
		ImGui::DragInt("CreateMaxParticles", &maxParticles_);
		// ボタンを押したときの処理
		if (ImGui::Button("CreateGroup")) {
			if (isGroupCreate) {
				emitters_[emitterNameText_] = std::move(CreateEmitterByType(nowEmitterTypeName_));
				if (!emitters_[emitterNameText_]) {
					nowEmitterTypeName_ = "Error";
					return;
				}
				emitters_[emitterNameText_]->Initialize(emitterNameText_, maxParticles_);
				emitters_[emitterNameText_]->ApplyGlobalVariables();
				emitters_[emitterNameText_]->InitializeGlobalVariables();
			}
		}
		ImGui::EndMenu();
	}
}

std::unique_ptr<BaseParticleEmitter> ParticleSystem::CreateEmitterByType(const std::string& typeName){
	if (typeName == "Sphere") {
		return std::make_unique<EmitterSphere>();
	}
	return nullptr;
}

void ParticleSystem::InitializeGlobalVariables(){

}

void ParticleSystem::ApplyGlobalVariables(){

}
