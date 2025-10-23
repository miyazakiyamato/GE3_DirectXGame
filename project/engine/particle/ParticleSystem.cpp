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

void ParticleSystem::SetParticleEmitter(std::unique_ptr<BaseParticleEmitter> emitter){
	emitters_[emitter->GetName()] = std::move(emitter);
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

void ParticleSystem::InitializeGlobalVariables(){

}

void ParticleSystem::ApplyGlobalVariables(){

}