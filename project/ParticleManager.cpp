#include "ParticleManager.h"

ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new ParticleManager;
	}
	return instance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon) {
	particleCommon_ = std::make_unique<ParticleCommon>();
	particleCommon_->Initialize(dxCommon);
}

void ParticleManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ParticleManager::DrawCommonSetting() {
	particleCommon_->DrawCommonSetting();
}

void ParticleManager::ChangeBlendMode(ParticleCommon::BlendMode blendMode) {
	particleCommon_->SetBlendMode(blendMode);
	particleCommon_->CreateGraphicsPipeline();
}