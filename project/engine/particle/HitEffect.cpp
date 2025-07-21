#include "HitEffect.h"
#include "ParticleManager.h"
#include "TimeManager.h"

void HitEffect::Initialize(const std::string& emitterName) {
	name_ = emitterName;
	std::string groupName = "aRing";
	emitterDates_[groupName].transform.scale = { 1.0f, 1.0f, 1.0f };
	emitterDates_[groupName].count = 1;
	ParticleManager::GetInstance()->CreateRingParticleGroup(groupName, 32, 1.0f, 0.2f);
	groupName = "bCross";
	emitterDates_[groupName].transform.scale = { 1.0f, 1.0f, 1.0f };
	emitterDates_[groupName].count = 4;
	ParticleManager::GetInstance()->CreateParticleGroup(groupName);
	groupName = "cCylinder";
	emitterDates_[groupName].transform.scale = { 1.0f, 1.0f, 1.0f };
	emitterDates_[groupName].count = 10;
	ParticleManager::GetInstance()->CreateCylinderParticleGroup(groupName, 32, 5.0f, 0.1f, 1.0f);
	groupName = "dCircle";
	emitterDates_[groupName].transform.scale = { 1.0f, 1.0f, 1.0f };
	emitterDates_[groupName].count = 10;
	ParticleManager::GetInstance()->CreateParticleGroup(groupName);
}
void HitEffect::Update(){
	for (auto& [name, data] : emitterDates_) {
		ParticleManager::ParticleGroup* group = ParticleManager::GetInstance()->GetParticleGroup(name);
		if (group == nullptr) { continue; }
		int count = 0;
		for (std::list<ParticleManager::Particle>::iterator it = group->particles.begin(); it != group->particles.end();) {
			ParticleManager::Particle& particle = *it;
			if (name == "aRing") {
				particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
				particle.transform.translate += particle.velocity * TimeManager::GetInstance()->deltaTime_;
				particle.transform.scale.x = 0.1f + (particle.currentTime / particle.lifeTime) * 0.9f;
				particle.transform.scale.y = 0.1f + (particle.currentTime / particle.lifeTime) * 0.9f;
				//particle.uvTransform.translate.x += 0.1f * TimeManager::GetInstance()->deltaTime_;
			}
			if (name == "bCross") {
				particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
				particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
				if (count % 2 == 0) {
					//particle.transform.scale.x = 1.0f - (particle.currentTime / particle.lifeTime);
					particle.transform.scale.y = (1.0f - (particle.currentTime / particle.lifeTime)) * 3.0f;
				}
				if (count % 2 == 1) {
					//particle.transform.scale.x = 1.0f - (particle.currentTime / particle.lifeTime);
					particle.transform.scale.x = (1.0f - (particle.currentTime / particle.lifeTime)) * 3.0f;
				}
			}
			if (name == "cCylinder") {
				particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
				particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
				particle.uvTransform.translate.x += 0.1f * TimeManager::GetInstance()->deltaTime_;
			}
			if (name == "dCircle") {
				particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
				particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
			}
			++count;
			++it;
		}

		if (!data.isEmitUpdate) { continue; }
		data.frequencyTime -= TimeManager::GetInstance()->deltaTime_;
		if (data.frequencyTime <= 0.0f) {
			data.frequencyTime = data.frequency;
			ParticleManager::GetInstance()->Emit(
				name,
				data.transform.translate,
				data.count
			);
		}
	}
}
void HitEffect::Emit(){
	for (const auto& [name, data] : emitterDates_) {
		ParticleManager::GetInstance()->Emit(name, emitterDates_[name].transform.translate + GetPosition(), data.count);
	}
}
;
