#include "BaseParticleEmitter.h"
#include <imgui.h>
#include "GlobalVariables.h"
#include "TextureManager.h"
#include "TimeManager.h"
#include "ParticleManager.h"

void BaseParticleEmitter::Initialize(const std::string& emitterName, uint32_t kMaxParticles){
	name_ = emitterName;
	particleManager_ = ParticleManager::GetInstance();
	dxCommon_ = ParticleManager::GetInstance()->GetDirectXCommon();
	srvUavManager_ = particleManager_->GetSrvUavManager();
	particleManager_->CreateParticleGroup(name_,kMaxParticles);
	globalVariables_ = GlobalVariables::GetInstance();
}

void BaseParticleEmitter::InitializeGlobalVariables(){

}

void BaseParticleEmitter::ApplyGlobalVariables(){

}

void BaseParticleEmitter::SetTexture(const std::string& textureName) {
	particleManager_->SetTexture(name_, textureName);
}

void BaseParticleEmitter::SetRing(const uint32_t& kDivide, const float& kOuterRadius, const float& kInnerRadius) {
	particleManager_->SetRing(name_, kDivide, kOuterRadius, kInnerRadius);
}

void BaseParticleEmitter::SetCylinder(const uint32_t& kDivide, const float& kTopRadius, const float& kBottomRadius, const float& kHeight) {
	particleManager_->SetCylinder(name_, kDivide, kTopRadius, kBottomRadius, kHeight);
}