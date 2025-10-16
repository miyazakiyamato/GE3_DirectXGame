#include "HitEffect.h"
#include "ParticleManager.h"
#include "TimeManager.h"

void HitEffect::Initialize(const std::string& emitterName) {
	BaseParticleEmitter::Initialize(emitterName);
	/*std::string groupName = "aRing";
	emitterDates_[groupName].count = 1;
	ParticleManager::GetInstance()->CreateRingParticleGroup(groupName, 32, 1.0f, 0.2f);
	groupName = "bCross";
	emitterDates_[groupName].count = 4;
	ParticleManager::GetInstance()->CreateParticleGroup(groupName);
	groupName = "cCylinder";
	emitterDates_[groupName].count = 10;
	ParticleManager::GetInstance()->CreateCylinderParticleGroup(groupName, 32, 5.0f, 0.1f, 1.0f);
	groupName = "dCircle";
	emitterDates_[groupName].count = 10;
	ParticleManager::GetInstance()->CreateParticleGroup(groupName);*/
}
void HitEffect::Update(){
	BaseParticleEmitter::Update();
	//for (auto& [name, data] : emitterDates_) {
	//	ParticleManager::ParticleGroup* group = ParticleManager::GetInstance()->GetParticleGroup(name);
	//	if (group == nullptr) { continue; }
	//	int count = 0;
	//	for (std::list<ParticleManager::Particle>::iterator it = group->particles.begin(); it != group->particles.end();) {
	//		ParticleManager::Particle& particle = *it;
	//		if (name == "aRing") {
	//			particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
	//			particle.transform.translate += particle.velocity * TimeManager::GetInstance()->deltaTime_;
	//			particle.transform.scale.x = 0.1f + (particle.currentTime / particle.lifeTime) * 0.9f;
	//			particle.transform.scale.y = 0.1f + (particle.currentTime / particle.lifeTime) * 0.9f;
	//			//particle.uvTransform.translate.x += 0.1f * TimeManager::GetInstance()->deltaTime_;
	//		}
	//		if (name == "bCross") {
	//			particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
	//			particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
	//			if (count % 2 == 0) {
	//				//particle.transform.scale.x = 1.0f - (particle.currentTime / particle.lifeTime);
	//				particle.transform.scale.y = (1.0f - (particle.currentTime / particle.lifeTime)) * 3.0f;
	//			}
	//			if (count % 2 == 1) {
	//				//particle.transform.scale.x = 1.0f - (particle.currentTime / particle.lifeTime);
	//				particle.transform.scale.x = (1.0f - (particle.currentTime / particle.lifeTime)) * 3.0f;
	//			}
	//		}
	//		if (name == "cCylinder") {
	//			particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
	//			particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
	//			particle.uvTransform.translate.x += 0.1f * TimeManager::GetInstance()->deltaTime_;
	//		}
	//		if (name == "dCircle") {
	//			particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
	//			particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
	//		}
	//		++count;
	//		++it;
	//	}

	//	
	//}
}
void HitEffect::Emit(){
	BaseParticleEmitter::Emit();
}

