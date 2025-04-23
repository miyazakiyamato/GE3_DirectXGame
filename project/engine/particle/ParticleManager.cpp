#include "ParticleManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include <numbers>
#include "PipelineManager.h"
#include "TimeManager.h"
#include "GlobalVariables.h"
#include <imgui.h>

ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance(){
	if (instance == nullptr) {
		instance = new ParticleManager;
	}
	return instance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager) {
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	//ランダムエンジンの初期化
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	randomEngine_ = randomEngine;

	//調整項目の初期化
	InitializeGlobalVariables();
	ApplyGlobalVariables();
	for (auto& [name, group] : particleGroupCreateDates_) {
		CreateParticleGroup(name, group->textureFilePath);
	}
}

void ParticleManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ParticleManager::Update() {
	//調整項目の更新
	ApplyGlobalVariables();

	Matrix4x4 worldMatrix;
	Matrix4x4 viewProjectionMatrix;
	Matrix4x4 billboardMatrix;
	Matrix4x4 worldViewProjectionMatrix;
	
	if (CameraManager::GetInstance()->GetCamera()) {
		viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
		billboardMatrix = CameraManager::GetInstance()->GetCamera()->GetWorldMatrix();
		billboardMatrix.m[3][0] = 0.0f;
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;
	}
	for (std::pair<const std::string, std::unique_ptr<ParticleGroup>>& pair : particleGroups) {
		ParticleGroup& group = *pair.second;
		int index = 0;
		for (std::list<Particle>::iterator it = group.particles.begin(); it != group.particles.end();) {
			Particle& particle = *it;

			particle.currentTime += TimeManager::GetInstance()->deltaTime_;
			particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
			particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
			if (particle.lifeTime <= particle.currentTime) {
				it = group.particles.erase(it);
				group.kNumInstance--;
				continue;
			}
			if (group.particleInitData.isBillboard) {
				worldMatrix = Matrix4x4::MakeScaleMatrix(particle.transform.scale) * billboardMatrix * Matrix4x4::MakeRotateZMatrix(particle.transform.rotate.z) * Matrix4x4::MakeTranslateMatrix(particle.transform.translate);
			} else {
				worldMatrix = Matrix4x4::MakeAffineMatrix(particle.transform.scale, particle.transform.rotate, particle.transform.translate);
			}
			worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
			group.instancingData[index].World = worldMatrix;
			group.instancingData[index].WVP = worldViewProjectionMatrix;
			group.instancingData[index].color = particle.color;

			++it;
			++index;
		}
	}
}

void ParticleManager::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 全てのパーティクルグループについて処理
	for (auto& [name, group] : particleGroups) {
		//パイプラインを設定
		PipelineManager::GetInstance()->DrawSetting(PipelineState::kParticle, group->blendMode_);
		commandList->IASetVertexBuffers(0, 1, &group->vertexBufferView);// VBVを設定
		commandList->IASetIndexBuffer(&group->indexBufferView);//IBVを設定
		// インスタンシングデータのSRVのDescriptorTableを設定
		srvManager_->SetGraphicsRootDescriptorTable(0, group->srvIndexForInstancing);
		//SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
		commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetSrvHandleGPU(group->materialData.textureFilePath));
		// DrawCall (インスタンシング描画)
		commandList->DrawIndexedInstanced(6, group->kNumInstance, 0, 0, 0);
	}
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath) {
	if (particleGroups.count(name) != 0) {
		return;
	}
	//assert(particleGroups.count(name) == 0 && "ParticleGroup with this name already exists.");

	// パーティクルグループの作成と初期化
	auto group = std::make_unique<ParticleGroup>();
	//.objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	group->materialData.textureFilePath = textureFilePath;

	group->kNumInstance = 0;

	group->kParticleVertexNum = 4;
	group->kParticleIndexNum = 6;
	// 頂点リソースの生成
	group->vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * group->kParticleVertexNum);

	// 頂点バッファビューの生成
	group->vertexBufferView.BufferLocation = group->vertexResource->GetGPUVirtualAddress();
	group->vertexBufferView.SizeInBytes = sizeof(VertexData) * group->kParticleVertexNum;
	group->vertexBufferView.StrideInBytes = sizeof(VertexData);
	// 頂点リソースに頂点データを書き込む
	group->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->vertexData));
	//テクスチャの頂点
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath);
	group->textureSize_.x = static_cast<float>(metadata.width);
	group->textureSize_.y = static_cast<float>(metadata.height);

	float tex_left = group->textureLeftTop_.x / metadata.width;
	float tex_right = (group->textureLeftTop_.x + group->textureSize_.x) / metadata.width;
	float tex_top = group->textureLeftTop_.y / metadata.height;
	float tex_bottom = (group->textureLeftTop_.y + group->textureSize_.y) / metadata.height;

	// 頂点データを設定（四角形を構成）
	group->vertexData[0] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { tex_left ,tex_bottom }, { 1.0f,1.0f,1.0f,1.0f } };//左下
	group->vertexData[1] = { { -0.5f,  0.5f, 0.0f, 1.0f }, { tex_left ,tex_top    }, { 1.0f,1.0f,1.0f,1.0f } };//左上
	group->vertexData[2] = { {  0.5f, -0.5f, 0.0f, 1.0f }, { tex_right,tex_bottom }, { 1.0f,1.0f,1.0f,1.0f } };//右下
	group->vertexData[3] = { {  0.5f,  0.5f, 0.0f, 1.0f }, { tex_right,tex_top    }, { 1.0f,1.0f,1.0f,1.0f } };//右上
	group->vertexResource->Unmap(0, nullptr);

	// インデックスリソースの生成
	group->indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * group->kParticleIndexNum);

	// インデックスバッファビューの生成
	group->indexBufferView.BufferLocation = group->indexResource->GetGPUVirtualAddress();
	group->indexBufferView.SizeInBytes = sizeof(uint32_t) * group->kParticleIndexNum;
	group->indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	group->indexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->indexData));
	group->indexData[0] = 0; group->indexData[1] = 1; group->indexData[2] = 2;
	group->indexData[3] = 1; group->indexData[4] = 3; group->indexData[5] = 2;
	group->indexResource->Unmap(0, nullptr);

	// TextureManagerからGPUハンドルを取得
	group->materialData.srvIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	group->instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kMaxInstance);
	//データを書き込む
	//書き込むためのアドレスを取得
	group->instancingResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&group->instancingData));
	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kMaxInstance; index++) {
		group->instancingData[index].World = Matrix4x4::MakeIdentity4x4();
		group->instancingData[index].WVP = Matrix4x4::MakeIdentity4x4();
		group->instancingData[index].color = { 1.0f,1.0f,1.0f,1.0f };//色を書き込む
	}

	group->srvIndexForInstancing = srvManager_->ALLocate();
	srvManager_->CreateSRVforStructuredBuffer(group->srvIndexForInstancing, group->instancingResource.Get(), kMaxInstance, sizeof(ParticleForGPU));

	auto it = particleGroupCreateDates_.find(name);
	if (it != particleGroupCreateDates_.end() && it->second) {
		group->particleInitData = it->second->particleInitData;
	}
	//group->particleInitData = particleGroupCreateDates_.find(name)->second->particleInitData;

	particleGroups[name] = std::move(group);
}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count) {
	if (particleGroups.count(name) == 0) {
		return;
	}
	//assert(particleGroups.count(name) > 0 && "ParticleGroup with this name does not exist.");
	
	ParticleGroup& group = *particleGroups[name];
	
	uint32_t nowInstance = group.kNumInstance;
	group.kNumInstance += count;
	if (group.kNumInstance + count >= kMaxInstance) {
		group.kNumInstance = kMaxInstance;
	}
	for (uint32_t i = nowInstance; i < group.kNumInstance; ++i) {
		Particle particle{};
		particle.transform.scale = Vector3::Random(randomEngine_, group.particleInitData.randomScaleMin, group.particleInitData.randomScaleMax);
		particle.transform.rotate = Vector3::Random(randomEngine_, group.particleInitData.randomRotateMin, group.particleInitData.randomRotateMax);
		particle.transform.translate = position;
		particle.velocity = Vector3::Random(randomEngine_, group.particleInitData.randomVelocityMin, group.particleInitData.randomVelocityMax);
		particle.color = Vector4::Random(randomEngine_, group.particleInitData.randomColorMin, group.particleInitData.randomColorMax);
		particle.lifeTime = group.particleInitData.lifeTime;
		group.particles.push_back(particle);
	}
}
//調整項目の初期化
void ParticleManager::InitializeGlobalVariables(){
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Particle";
	// グループを追加
	globalVariables->CreateGroup(groupName);
	uint32_t objectIDIndex = 0;
	while (globalVariables->GetValue<std::string>(groupName, "\x01Name" + std::to_string(objectIDIndex)) != "") {
		std::string name = globalVariables->GetValue<std::string>(groupName, "\x01Name" + std::to_string(objectIDIndex));
		if (particleGroups.count(name) == 0) {
			particleGroupCreateDates_[name] = std::make_unique<ParticleGroupCreateData>();
		}
		++objectIDIndex;
	}
	objectIDIndex = 0;
	for (auto& [name, group] : particleGroupCreateDates_) {
		globalVariables->AddItem(groupName, "\x01Name" + std::to_string(objectIDIndex), name);
		globalVariables->AddItem(groupName, "\x02TextureFilePath" + std::to_string(objectIDIndex), group->textureFilePath);
		globalVariables->AddItem(groupName, "\x03RandomScaleMax" + std::to_string(objectIDIndex), group->particleInitData.randomScaleMax);
		globalVariables->AddItem(groupName, "\x04RandomScaleMin" + std::to_string(objectIDIndex), group->particleInitData.randomScaleMin);
		globalVariables->AddItem(groupName, "\x05RandomRotateMax" + std::to_string(objectIDIndex), group->particleInitData.randomRotateMax);
		globalVariables->AddItem(groupName, "\x06RandomRotateMin" + std::to_string(objectIDIndex), group->particleInitData.randomRotateMin);
		globalVariables->AddItem(groupName, "\x07RandomVelocityMax" + std::to_string(objectIDIndex), group->particleInitData.randomVelocityMax);
		globalVariables->AddItem(groupName, "\x08RandomVelocityMin" + std::to_string(objectIDIndex), group->particleInitData.randomVelocityMin);
		globalVariables->AddItem(groupName, "\x09RandomColorMax" + std::to_string(objectIDIndex), group->particleInitData.randomColorMax);
		globalVariables->AddItem(groupName, "\x0ARandomColorMin" + std::to_string(objectIDIndex), group->particleInitData.randomColorMin);
		globalVariables->AddItem(groupName, "\x0BLifeTime" + std::to_string(objectIDIndex), group->particleInitData.lifeTime);
		globalVariables->AddItem(groupName, "\x0CIsBillboard" + std::to_string(objectIDIndex), group->particleInitData.isBillboard);
		++objectIDIndex;
	}
}
// 調整項目の適用
void ParticleManager::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	std::string groupName = "Particle";
	uint32_t objectIDIndex = 0;
	for (auto& [name, group] : particleGroupCreateDates_) {
		group->name = globalVariables->GetValue<std::string>(groupName, "\x01Name" + std::to_string(objectIDIndex));
		group->textureFilePath = globalVariables->GetValue<std::string>(groupName, "\x02TextureFilePath" + std::to_string(objectIDIndex));
		group->particleInitData.randomScaleMax = globalVariables->GetValue<Vector3>(groupName, "\x03RandomScaleMax" + std::to_string(objectIDIndex));
		group->particleInitData.randomScaleMin = globalVariables->GetValue<Vector3>(groupName, "\x04RandomScaleMin" + std::to_string(objectIDIndex));
		group->particleInitData.randomRotateMax = globalVariables->GetValue<Vector3>(groupName, "\x05RandomRotateMax" + std::to_string(objectIDIndex));
		group->particleInitData.randomRotateMin = globalVariables->GetValue<Vector3>(groupName, "\x06RandomRotateMin" + std::to_string(objectIDIndex));
		group->particleInitData.randomVelocityMax = globalVariables->GetValue<Vector3>(groupName, "\x07RandomVelocityMax" + std::to_string(objectIDIndex));
		group->particleInitData.randomVelocityMin = globalVariables->GetValue<Vector3>(groupName, "\x08RandomVelocityMin" + std::to_string(objectIDIndex));
		group->particleInitData.randomColorMax = globalVariables->GetValue<Vector4>(groupName, "\x09RandomColorMax" + std::to_string(objectIDIndex));
		group->particleInitData.randomColorMin = globalVariables->GetValue<Vector4>(groupName, "\x0ARandomColorMin" + std::to_string(objectIDIndex));
		group->particleInitData.lifeTime = globalVariables->GetValue<float>(groupName, "\x0BLifeTime" + std::to_string(objectIDIndex));
		group->particleInitData.isBillboard = globalVariables->GetValue<bool>(groupName, "\x0CIsBillboard" + std::to_string(objectIDIndex));
		++objectIDIndex;
	}
}

void ParticleManager::UpdateGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Particle";
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
		if (ImGui::InputText("Input Text", buffer, IM_ARRAYSIZE(buffer))) {
			// 入力が変更された場合に反映
			reflectedText = buffer;
		}

		// 入力された文字列を表示
		ImGui::Text("Reflected Text: %s", reflectedText.c_str());
		// ボタンを押したときの処理
		if (ImGui::Button("CreateGroup")) {
			if (reflectedText.empty()) {
				ImGui::Text("Error: Group name cannot be empty.");
			} else if (particleGroupCreateDates_.count(reflectedText) != 0) {
				ImGui::Text("Error: Group name already exists.");
			} else {
				particleGroupCreateDates_[reflectedText] = std::make_unique<ParticleGroupCreateData>();
				InitializeGlobalVariables();
				CreateParticleGroup(reflectedText, particleGroupCreateDates_[reflectedText]->textureFilePath);
			}
		}
		//パーティクル初期化データの更新
		for (auto& [name, group] : particleGroups) {
			auto it = particleGroupCreateDates_.find(name);
			if (it != particleGroupCreateDates_.end() && it->second) {
				group->particleInitData = it->second->particleInitData;
			}
		}
		uint32_t objectIDIndex = 0;
		for (auto& [name, group] : particleGroupCreateDates_) {
			//テクスチャのキーを取得
			std::string textureName = name + ": Now Texture";
			std::vector<std::string> textureState = TextureManager::GetInstance()->GetKeys();
			auto it = particleGroups.find(name);
			if (it != particleGroups.end() && it->second) {
				//テクスチャ選択
				std::string textureItem_selected_idx = it->second->materialData.textureFilePath;
				const char* currentItem = textureItem_selected_idx.c_str();
				if (ImGui::BeginCombo((textureName + name).c_str(), currentItem)) {
					for (int i = 0; i < textureState.size(); ++i) {
						bool isSelected = (textureItem_selected_idx == textureState[i]);
						if (ImGui::Selectable(textureState[i].c_str(), isSelected)) {
							it->second->materialData.textureFilePath = textureState[i];

							group->textureFilePath = textureState[i];
							globalVariables->SetValue(groupName, "\x02TextureFilePath" + std::to_string(objectIDIndex), group->textureFilePath);
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			//パーティクルの発生
			if (ImGui::Button(std::string(name + ": Emit").c_str())) {
				if (particleGroupCreateDates_.count(name) == 0) {
					ImGui::Text("Error: Group name is empty.");
				} else {
					Emit(name, Vector3(0.0f, 0.0f, 0.0f), 1);
				}
			}
			++objectIDIndex;
		}

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
