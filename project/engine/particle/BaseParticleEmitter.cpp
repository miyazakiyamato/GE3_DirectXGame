#include "BaseParticleEmitter.h"
#include <imgui.h>
#include "GlobalVariables.h"
#include "TextureManager.h"
#include "TimeManager.h"
#include "ParticleManager.h"

void BaseParticleEmitter::Initialize(const std::string& emitterName){
	name_ = emitterName;
	//調整項目の初期化
	ApplyGlobalVariables();
	InitializeGlobalVariables();
	for (auto& [name, data] : emitterDates_) {
		if (data.kDivide == 0) {
			data.kDivide = 32; // デフォルトの分割数
		}
		if (data.particleInitData.particleType == "cylinder") {	
			ParticleManager::GetInstance()->CreateCylinderParticleGroup(name, data.kDivide, data.kTopRadius, data.kBottomRadius, data.kHeight);
		} else if (data.particleInitData.particleType == "ring") {
			ParticleManager::GetInstance()->CreateRingParticleGroup(name, data.kDivide, data.kOuterRadius, data.kInnerRadius);
		} else {
			ParticleManager::GetInstance()->CreateParticleGroup(name);
		}
	}
}

void BaseParticleEmitter::Update(){
	//調整項目の更新
	ApplyGlobalVariables();
	for (auto& [name, data] : emitterDates_) {
		if (!data.isEmitUpdate) { continue; }
		data.frequencyTime -= TimeManager::GetInstance()->deltaTime_;
		if (data.frequencyTime <= 0.0f) {
			data.frequencyTime = data.frequency;
			ParticleManager::GetInstance()->Emit(
				name,
				GetPosition(),
				data.particleInitData
			);
		}
	}
}

void BaseParticleEmitter::Emit(){
	for (const auto& [name, data] : emitterDates_) {
		ParticleManager::GetInstance()->Emit(name, GetPosition(), data.particleInitData);
	}
}

void BaseParticleEmitter::UpdateGlobalVariables(){
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = name_.c_str();
	//std::string blendName = "Now Blend";
	//std::vector<std::string> blendState{
	//	"None",      //!< ブレンドなし
	//	"Normal",    //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	//	"Add",       //!< 加算。Src * SrcA + Dest * 1
	//	"Subtract",  //!< 減算。Dest * 1 - Src * SrcA
	//	"Multiply",  //!< 乗算。Src * 0 + Dest * Src
	//	"Screen", };
	if (ImGui::BeginMenu(groupName)) {
		// テキスト入力ボックス
		if (ImGui::InputText("Input GroupNameText", buffer, IM_ARRAYSIZE(buffer))) {
			// 入力が変更された場合に反映
			groupNameText = buffer;
		}
		if (ImGui::InputText("Input TypeNameText", buffer2, IM_ARRAYSIZE(buffer2))) {
			// 入力が変更された場合に反映
			typeNameText = buffer2;
		}
		bool isGroupCreate = false;
		// 入力された文字列を表示
		ImGui::Text("Reflected Text: %s", groupNameText.c_str());
		if (groupNameText.empty()) {
			ImGui::Text("Error: Group name cannot be empty.");
		} else if (emitterDates_.count(groupNameText) != 0) {
			ImGui::Text("Error: Group name already exists.");
		} else {
			isGroupCreate = true;
		}
		// ボタンを押したときの処理
		if (ImGui::Button("CreateGroup")) {
			if (isGroupCreate) {
				/*emitterDates_[groupNameText] = std::make_unique<ParticleGroupCreateData>();
				InitializeGlobalVariables();
				if (typeNameText == "cylinder") {
					CreateCylinderParticleGroup(groupNameText, 32, 1.0f, 1.0f, 3.0f);
				} else if (typeNameText == "ring") {
					CreateRingParticleGroup(groupNameText, 32, 1.0f, 0.2f);
				} else {
					CreateParticleGroup(groupNameText);
				}*/
			}
		}
		//uint32_t objectIDIndex = 0;
		//for (auto& [name, group] : emitterDates_) {
		//	//テクスチャのキーを取得
		//	std::string textureName = name + ": Now Texture";
		//	std::vector<std::string> textureState = TextureManager::GetInstance()->GetKeys();
		//	auto it = particleGroups.find(name);
		//	if (it != particleGroups.end() && it->second) {
		//		//テクスチャ選択
		//		std::string textureItem_selected_idx = it->second->materialData.textureFilePath;
		//		const char* currentItem = textureItem_selected_idx.c_str();
		//		if (ImGui::BeginCombo((textureName + name).c_str(), currentItem)) {
		//			for (int i = 0; i < textureState.size(); ++i) {
		//				bool isSelected = (textureItem_selected_idx == textureState[i]);
		//				if (ImGui::Selectable(textureState[i].c_str(), isSelected)) {
		//					it->second->materialData.textureFilePath = textureState[i];

		//					group->textureFilePath = textureState[i];
		//					globalVariables->SetValue(groupName, "TextureFilePath" + std::to_string(objectIDIndex), group->textureFilePath);
		//				}
		//				if (isSelected) {
		//					ImGui::SetItemDefaultFocus();
		//				}
		//			}
		//			ImGui::EndCombo();
		//		}
		//		globalVariables->SetValue(groupName, "ParticleType" + std::to_string(objectIDIndex), group->particleType);
		//	}
		//	//パーティクルの発生
		//	if (ImGui::Button(std::string(name + ": Emit").c_str())) {
		//		if (emitterDates_.count(name) == 0) {
		//			ImGui::Text("Error: Group name is empty.");
		//		} else {
		//			Emit(name, Vector3(0.0f, 0.0f, 0.0f), 1);
		//		}
		//	}
		//	++objectIDIndex;
		//}

		//	int particleItem_selected_idx = static_cast<int>(ParticleManager::GetInstance()->GetBlendMode(particleEmitter_->GetName()));
		//	const char* currentItem = blendState[particleItem_selected_idx].c_str();
		//	if (ImGui::BeginCombo((blendName + particleEmitter_->GetName()).c_str(), currentItem)) {
		//		for (int i = 0; i < blendState.size(); ++i) {
		//			bool isSelected = (particleItem_selected_idx == i);
		//			if (ImGui::Selectable(blendState[i].c_str(), isSelected)) {
		//				particleItem_selected_idx = i;
		//				ParticleManager::GetInstance()->SetBlendMode(particleEmitter_->GetName(), static_cast<BlendMode>(i));
		//			}
		//			if (isSelected) {
		//				ImGui::SetItemDefaultFocus();
		//			}
		//		}
		//		ImGui::EndCombo();
		//	}

		//	Vector3 position = particleEmitter_->GetPosition();
		//	ImGui::DragFloat2("particleEmitter_.Translate", &position.x, 0.1f);
		//	/*if (position.y > 640.0f) {
		//		position.y = 640.0f;
		//	}*/

		//	//Vector3 rotation = particleEmitter_->GetRotation();
		//	//ImGui::SliderAngle("particleEmitter_.Rotate", &rotation.x);

		//	//Vector3 size = particleEmitter_->GetSize();
		//	//ImGui::DragFloat2("particleEmitter_.Scale", &size.x, 0.1f);
		//	//if (size.y > 360.0f) {
		//	//	size.y = 360.0f;
		//	//}

		//	int count = particleEmitter_->GetCount();
		//	ImGui::DragInt("particleEmitter_.count", &count, 1, 0, 1000);

		//	float frequency = particleEmitter_->GetFrequency();
		//	ImGui::DragFloat("particleEmitter_.frequency", &frequency, 0.1f);

		//	if (ImGui::Button("ParticleEmit", { 100,50 })) {
		//		particleEmitter_->Emit();
		//	}

		//	bool isEmitUpdate = particleEmitter_->GetIsEmitUpdate();
		//	ImGui::Checkbox("IsEmitUpdate", &isEmitUpdate);

		//	particleEmitter_->SetPosition(position);
		//	//particleEmitter_->SetRotation(rotation);
		//	//particleEmitter_->SetSize(size);
		//	particleEmitter_->SetCount(count);
		//	particleEmitter_->SetFrequency(frequency);
		//	particleEmitter_->SetIsEmitUpdate(isEmitUpdate);

		//	//ImGui::Checkbox("IsAccelerationField", &isAccelerationField);
		ImGui::EndMenu();
	}
}

void BaseParticleEmitter::InitializeGlobalVariables(){
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = name_.c_str();
	
	uint32_t objectIDIndex = 0;
	for (auto& [name, data] : emitterDates_) {
		// まとめてヘッダーグループを設定
		globalVariables->SetHeaderGroup(groupName, name);
		globalVariables->AddItem(groupName, "Name" + std::to_string(objectIDIndex), name);
		globalVariables->AddItem(groupName, "TextureFilePath" + std::to_string(objectIDIndex), data.particleInitData.textureFilePath);
		globalVariables->AddItem(groupName, "ParticleType" + std::to_string(objectIDIndex), data.particleInitData.particleType);
		globalVariables->AddItem(groupName, "RandomTransformMax" + std::to_string(objectIDIndex), data.particleInitData.randomTransformMax);
		globalVariables->AddItem(groupName, "RandomTransformMin" + std::to_string(objectIDIndex), data.particleInitData.randomTransformMin);
		globalVariables->AddItem(groupName, "RandomColorMax" + std::to_string(objectIDIndex), data.particleInitData.randomColorMax);
		globalVariables->AddItem(groupName, "RandomColorMin" + std::to_string(objectIDIndex), data.particleInitData.randomColorMin);
		globalVariables->AddItem(groupName, "LifeTime" + std::to_string(objectIDIndex), data.particleInitData.lifeTime);
		globalVariables->AddItem(groupName, "IsBillboard" + std::to_string(objectIDIndex), data.particleInitData.isBillboard);
		globalVariables->AddItem(groupName, "Count" + std::to_string(objectIDIndex), (int32_t)data.particleInitData.count);
		globalVariables->AddItem(groupName, "Frequency" + std::to_string(objectIDIndex), data.frequency);
		globalVariables->AddItem(groupName, "IsEmitUpdate" + std::to_string(objectIDIndex), data.isEmitUpdate);
		if (data.particleInitData.particleType == "cylinder") {
			globalVariables->AddItem(groupName, "kDivide" + std::to_string(objectIDIndex), (int32_t)data.kDivide);
			globalVariables->AddItem(groupName, "kTopRadius" + std::to_string(objectIDIndex), data.kTopRadius);
			globalVariables->AddItem(groupName, "kBottomRadius" + std::to_string(objectIDIndex), data.kBottomRadius);
			globalVariables->AddItem(groupName, "kHeight" + std::to_string(objectIDIndex), data.kHeight);
		} else if (data.particleInitData.particleType == "ring") {
			globalVariables->AddItem(groupName, "kDivide" + std::to_string(objectIDIndex), (int32_t)data.kDivide);
			globalVariables->AddItem(groupName, "kOuterRadius" + std::to_string(objectIDIndex), data.kOuterRadius);
			globalVariables->AddItem(groupName, "kInnerRadius" + std::to_string(objectIDIndex), data.kInnerRadius);
		}
		globalVariables->EndHeaderGroup(groupName);
		++objectIDIndex;
	}
}

void BaseParticleEmitter::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	std::string groupName = name_.c_str();
	// グループを追加
	globalVariables->CreateGroup(groupName);
	uint32_t objectIDIndex = 0;

	while (globalVariables->GetValue<std::string>(groupName, "Name" + std::to_string(objectIDIndex)) != "") {
		std::string name = globalVariables->GetValue<std::string>(groupName, "Name" + std::to_string(objectIDIndex));
		if (emitterDates_.count(name) == 0) {
			emitterDates_[name] = EmitterData();
		}
		++objectIDIndex;
	}
	objectIDIndex = 0;
	for (auto& [name, data] : emitterDates_) {
		data.particleInitData.textureFilePath = globalVariables->GetValue<std::string>(groupName, "TextureFilePath" + std::to_string(objectIDIndex));
		data.particleInitData.particleType = globalVariables->GetValue<std::string>(groupName, "ParticleType" + std::to_string(objectIDIndex));
		data.particleInitData.randomTransformMax = globalVariables->GetValue<Transform>(groupName, "RandomTransformMax" + std::to_string(objectIDIndex));
		data.particleInitData.randomTransformMin = globalVariables->GetValue<Transform>(groupName, "RandomTransformMin" + std::to_string(objectIDIndex));
		data.particleInitData.randomColorMax = globalVariables->GetValue<Vector4>(groupName, "RandomColorMax" + std::to_string(objectIDIndex));
		data.particleInitData.randomColorMin = globalVariables->GetValue<Vector4>(groupName, "RandomColorMin" + std::to_string(objectIDIndex));
		data.particleInitData.lifeTime = globalVariables->GetValue<float>(groupName, "LifeTime" + std::to_string(objectIDIndex));
		data.particleInitData.isBillboard = globalVariables->GetValue<bool>(groupName, "IsBillboard" + std::to_string(objectIDIndex));
		data.particleInitData.count = (uint32_t)globalVariables->GetValue<int32_t>(groupName, "Count" + std::to_string(objectIDIndex));
		data.frequency = globalVariables->GetValue<float>(groupName, "Frequency" + std::to_string(objectIDIndex));
		data.isEmitUpdate = globalVariables->GetValue<bool>(groupName, "IsEmitUpdate" + std::to_string(objectIDIndex));
		if (data.particleInitData.particleType == "cylinder") {
			data.kDivide = (uint32_t)globalVariables->GetValue<int32_t>(groupName, "kDivide" + std::to_string(objectIDIndex));
			data.kTopRadius = globalVariables->GetValue<float>(groupName, "kTopRadius" + std::to_string(objectIDIndex));
			data.kBottomRadius = globalVariables->GetValue<float>(groupName, "kBottomRadius" + std::to_string(objectIDIndex));
			data.kHeight = globalVariables->GetValue<float>(groupName, "kHeight" + std::to_string(objectIDIndex));
		} else if (data.particleInitData.particleType == "ring") {
			data.kDivide = (uint32_t)globalVariables->GetValue<int32_t>(groupName, "kDivide" + std::to_string(objectIDIndex));
			data.kOuterRadius = globalVariables->GetValue<float>(groupName, "kOuterRadius" + std::to_string(objectIDIndex));
			data.kInnerRadius = globalVariables->GetValue<float>(groupName, "kInnerRadius" + std::to_string(objectIDIndex));
		}
		++objectIDIndex;
	}
}
