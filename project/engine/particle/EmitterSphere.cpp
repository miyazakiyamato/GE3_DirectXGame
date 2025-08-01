#include "EmitterSphere.h"
#include "ParticleManager.h"
#include "TimeManager.h"
#include "PipelineManager.h"
#include "SrvUavManager.h"

void EmitterSphere::Initialize(const std::string& emitterName) {
	name_ = emitterName;
	particleManager_ = ParticleManager::GetInstance();
	dxCommon_ = ParticleManager::GetInstance()->GetDirectXCommon();
	srvUavManager_ = particleManager_->GetSrvUavManager();
	particleManager_->CreateParticleGroup(name_);
	computeShaderPipelineName_ = PipelineManager::GetInstance()->CreateComputePipelineState("EmitParticle");
	
	//worldViewProjection用のリソースを作成
	emitterDataResource_ = dxCommon_->CreateBufferResource(sizeof(EmitterData));
	emitterDataResource_->Map(0, nullptr, reinterpret_cast<void**>(&emitterData_));
	// エミッターのデータを初期化
	emitterData_->translate = { 0.0f, 0.0f, 0.0f }; // 初期位置
	emitterData_->radius = 1.0f; // 初期半径
	emitterData_->count = 10; // 初期射出数
	emitterData_->frequency = 0.5f; // 初期射出間隔（秒）
	emitterData_->frequencyTime = 0.0f; // 初期射出間隔調整用
	emitterData_->emit = 0; // 初期射出許可（0:許可しない、1:許可する）
}
void EmitterSphere::Update(){
	emitterData_->frequencyTime += TimeManager::deltaTime_;
	if (emitterData_->frequency <= emitterData_->frequencyTime) {
		emitterData_->frequencyTime -= emitterData_->frequency;
		emitterData_->emit = 1;
	} else {
		emitterData_->emit = 0;
	}
	Emit();
}
void EmitterSphere::Emit(){
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
	// freeCounterのリソースをUAVに設定
	srvUavManager_->SetComputeRootDescriptorTable(3, group->freeCounterUAVIndex);
	// Compute Shaderを実行
	commandList->Dispatch(1, 1, 1);
}