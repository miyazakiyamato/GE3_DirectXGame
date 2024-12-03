#include "ParticleEmitter.h"
#include "ParticleManager.h"

void ParticleEmitter::Emit(){

}

void ParticleEmitter::Initialize(){
	ParticleManager::GetInstance()->CreateParticleGroup("uvChecker", "resources/uvChecker.png");
}

void ParticleEmitter::Update(){
	frequencyTime_ -= kDeltaTime_;
	if (frequencyTime_ <= 0.0f) {
		frequencyTime_ = frequency_;
		ParticleManager::GetInstance()->Emit("uvChecker", transform.translate, count_);
	}
}
