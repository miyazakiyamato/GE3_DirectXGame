#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "TimeManager.h"

void ParticleEmitter::Initialize(const std::string name, const std::string textureFilePath){
	name_ = name;
	textureFilePath_ = textureFilePath;
	ParticleManager::GetInstance()->CreateParticleGroup(name_, textureFilePath_);
}

void ParticleEmitter::Update(){
	if (!isEmitUpdate_) { return; }
	frequencyTime_ -= TimeManager::GetInstance()->deltaTime_;
	if (frequencyTime_ <= 0.0f) {
		frequencyTime_ = frequency_;
		ParticleManager::GetInstance()->Emit(name_, transform_.translate, count_);
	}
}

void ParticleEmitter::Emit(){
	ParticleManager::GetInstance()->Emit(name_, transform_.translate, count_);
}