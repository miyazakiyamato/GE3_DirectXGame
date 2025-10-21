#include "ParticleSystem.h"
#include "ParticleManager.h"

void ParticleSystem::Finalize() {
	// すべてのエミッターを削除
	for (auto& emitter : emitters_) {
		emitter.second.reset();
	}
	emitters_.clear();
}

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

BaseParticleEmitter* ParticleSystem::GetParticleEmitter(const std::string& emitterName){
	if (emitters_.find(emitterName) != emitters_.end()) {
		return emitters_[emitterName].get();
	}
	return nullptr;
}

void ParticleSystem::Emit(const std::string& emitterName){
	emitters_[emitterName]->Emit();
}

void ParticleSystem::UpdateGlobalVariables(){
	/*for (const auto& [name, emitter] : emitters_) {
		emitter->UpdateGlobalVariables();
	}*/
}
