#include "EmitterSphere.h"
#include "ParticleManager.h"
#include "TimeManager.h"
#include "PipelineManager.h"
#include "SrvUavManager.h"
#include "GlobalVariables.h"

void EmitterSphere::Initialize(const std::string& emitterName, uint32_t kMaxParticles) {
	BaseParticleEmitter::Initialize(emitterName,kMaxParticles);
	computeShaderPipelineName_ = PipelineManager::GetInstance()->CreateComputePipelineState("EmitParticle");
	
	// worldViewProjection用のリソースを作成
	emitterDataResource_ = dxCommon_->CreateBufferResource(sizeof(EmitterData));
	emitterDataResource_->Map(0, nullptr, reinterpret_cast<void**>(&emitterData_));

	// エミッターのデータを初期化
	emitterData_->translate = { 0.0f, 1.0f, 0.0f }; // 初期位置
	emitterData_->radius = 1.0f; // 初期半径
	emitterData_->count = 10; // 初期射出数
	emitterData_->frequency = 1.0f; // 初期射出間隔（秒）
	emitterData_->frequencyTime = 0.0f; // 初期射出間隔調整用
	emitterData_->emit = 0; // 初期射出許可（0:許可しない、1:許可する）
	emitterData_->isBillboard = 0; // ビルボードの有無（0:無、1:有）
	emitterData_->isEmitUpdate = 0;// 連続発生するか（0:しない、1:する）
	emitterData_->startColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // 色
	emitterData_->endColor = { 1.0f, 1.0f, 1.0f, 0.0f }; // 色
	emitterData_->rlifeTimeMin = 0.1f; // random寿命最低値
	emitterData_->rlifeTimeMax = 3.0f; // random寿命最高値
	emitterData_->startScale = { 0.5f, 0.5f, 0.5f }; // スケール開始時の値
	emitterData_->endScale = { 1.0f, 1.0f, 1.0f }; // スケール終了時の値
	emitterData_->rVelocityMin = { -0.5f, -0.5f, -0.5f }; // random速度最低値
	emitterData_->rVelocityMax = { 0.5f, 0.5f, 0.5f }; // random速度最高値
	emitterData_->startUvTranslate = { 0.0f, 0.0f }; // uvTranslate開始時の値
	emitterData_->endUvTranslate = { 1.0f, 1.0f }; // uvTranslate終了時の値
}
void EmitterSphere::Update(){
	if (emitterData_->isEmitUpdate == 1) {
		emitterData_->frequencyTime += TimeManager::deltaTime_;
	}
	if (emitterData_->frequency <= emitterData_->frequencyTime) {
		emitterData_->frequencyTime = 0;
		emitterData_->emit = 1;
	} else {
		emitterData_->emit = 0;
	}

	// Emit
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	ParticleManager::ParticleGroup* group = particleManager_->GetParticleGroup(name_);
	srvUavManager_->PreDraw();
	// コンピュートパイプライン設定
	PipelineManager::GetInstance()->DrawSettingCS(computeShaderPipelineName_);
	// UAVをルートシグネチャに設定
	srvUavManager_->SetComputeRootDescriptorTable(0, group->particleUavIndex);
	// エミッターのデータをCBufferに設定
	commandList->SetComputeRootConstantBufferView(1, emitterDataResource_->GetGPUVirtualAddress());
	// フレームごとの時間情報をCBufferに設定
	commandList->SetComputeRootConstantBufferView(2, TimeManager::GetInstance()->GetPerFrameResource()->GetGPUVirtualAddress());
	// freeListのリソースをUAVに設定
	srvUavManager_->SetComputeRootDescriptorTable(3, group->freeListIndexUAVIndex);
	srvUavManager_->SetComputeRootDescriptorTable(4, group->freeListUAVIndex);
	//パーティクルの最大数のリソースをCBuufferに設定
	commandList->SetComputeRootConstantBufferView(5, group->maxParticlesResource->GetGPUVirtualAddress());
	// Compute Shaderを実行
	commandList->Dispatch(1, 1, 1);
}
void EmitterSphere::Emit(){
	emitterData_->frequencyTime = emitterData_->frequency;
}

void EmitterSphere::InitializeGlobalVariables(){
	BaseParticleEmitter::InitializeGlobalVariables();
	// グループを追加
	std::string groupName = name_;
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables_->AddItem(groupName, "Radius", emitterData_->radius);
	globalVariables_->AddItem(groupName, "Count", (int)emitterData_->count);
	globalVariables_->AddItem(groupName, "Frequency", emitterData_->frequency);
	globalVariables_->AddItem(groupName, "IsBillboard", (bool)emitterData_->isBillboard);
	globalVariables_->AddItem(groupName, "IsEmitUpdate", (bool)emitterData_->isEmitUpdate);
	globalVariables_->AddItem(groupName, "StartColor", emitterData_->startColor);
	globalVariables_->AddItem(groupName, "EndColor", emitterData_->endColor);
	globalVariables_->AddItem(groupName, "RLifeTimeMin", emitterData_->rlifeTimeMin);
	globalVariables_->AddItem(groupName, "RLifeTimeMax", emitterData_->rlifeTimeMax);
	globalVariables_->AddItem(groupName, "StartScale", emitterData_->startScale);
	globalVariables_->AddItem(groupName, "EndScale", emitterData_->endScale);
	globalVariables_->AddItem(groupName, "RVelocityMin", emitterData_->rVelocityMin);
	globalVariables_->AddItem(groupName, "RVelocityMax", emitterData_->rVelocityMax);
	globalVariables_->AddItem(groupName, "StartUvTranslate", emitterData_->startUvTranslate);
	globalVariables_->AddItem(groupName, "EndUvTranslate", emitterData_->endUvTranslate);
}

void EmitterSphere::ApplyGlobalVariables(){
	BaseParticleEmitter::ApplyGlobalVariables();
	std::string groupName = name_;
	emitterData_->radius = globalVariables_->GetValue<float>(groupName, "Radius");
	emitterData_->count = (uint32_t)globalVariables_->GetValue<int>(groupName, "Count");
	emitterData_->frequency = globalVariables_->GetValue<float>(groupName, "Frequency");
	emitterData_->isBillboard = globalVariables_->GetValue<bool>(groupName, "IsBillboard");
	emitterData_->isEmitUpdate = globalVariables_->GetValue<bool>(groupName, "IsEmitUpdate");
	emitterData_->startColor = globalVariables_->GetValue<Vector4>(groupName, "StartColor");
	emitterData_->endColor = globalVariables_->GetValue<Vector4>(groupName, "EndColor");
	emitterData_->rlifeTimeMin = globalVariables_->GetValue<float>(groupName, "RLifeTimeMin");
	emitterData_->rlifeTimeMax = globalVariables_->GetValue<float>(groupName, "RLifeTimeMax");
	emitterData_->startScale = globalVariables_->GetValue<Vector3>(groupName, "StartScale");
	emitterData_->endScale = globalVariables_->GetValue<Vector3>(groupName, "EndScale");
	emitterData_->rVelocityMin = globalVariables_->GetValue<Vector3>(groupName, "RVelocityMin");
	emitterData_->rVelocityMax = globalVariables_->GetValue<Vector3>(groupName, "RVelocityMax");
	emitterData_->startUvTranslate = globalVariables_->GetValue<Vector2>(groupName, "StartUvTranslate");
	emitterData_->endUvTranslate = globalVariables_->GetValue<Vector2>(groupName, "EndUvTranslate");
}
