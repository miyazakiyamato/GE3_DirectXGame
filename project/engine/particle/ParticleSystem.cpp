#include "ParticleSystem.h"
#include "ParticleManager.h"

void ParticleSystem::Update(){
	for (const auto& [name, emitter] : emitters_) {
		emitter->Update();
	}
	ParticleManager::GetInstance()->Update();
}

void ParticleSystem::Draw(){
	ParticleManager::GetInstance()->Draw();
}

void ParticleSystem::CreateParticleEmitter(const std::string& emitterName, std::unique_ptr<BaseParticleEmitter> emitter){
	emitters_[emitterName] = std::move(emitter);
	emitters_[emitterName]->Initialize(emitterName);
}

void ParticleSystem::Emit(const std::string& emitterName){
	emitters_[emitterName]->Emit();
}
