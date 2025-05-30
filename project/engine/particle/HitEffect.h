#pragma once
#include "BaseParticleEmitter.h"

class HitEffect : public BaseParticleEmitter{
	void Initialize(const std::string& emitterName) override;
	void Update() override;
	void Emit() override;
public:

};

