#include "ParticleEmitter.h"
#include "ParticleManager.h"

void ParticleEmitter::Initialize(const std::string name, const std::string textureFilePath){
	name_ = name;
	textureFilePath_ = textureFilePath;
	ParticleManager::GetInstance()->CreateParticleGroup(name_, textureFilePath_);
}

void ParticleEmitter::Update(){
	frequencyTime_ -= kDeltaTime_;
	if (frequencyTime_ <= 0.0f) {
		frequencyTime_ = frequency_;
		ParticleManager::GetInstance()->Emit(name_, transform_.translate, count_);
	}
}

void ParticleEmitter::Emit(){
	ParticleManager::GetInstance()->Emit(name_, transform_.translate, count_);
}