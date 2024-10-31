#include "Particle.h"
#include "ParticleManager.h"
#include "CameraManager.h"
#include "TextureManager.h"


void Particle::Initialize(std::string textureFilePath) {
	currentTime_ = 0;
}

void Particle::Update() {
	if (lifeTime_ <= currentTime_) {
		isAlive_ = false;
		return;
	}
	currentTime_ += kDeltaTime_;
	color_.w = 1.0f - (currentTime_ / lifeTime_);
	transform_.translate = transform_.translate + velocity_ * kDeltaTime_;

}

void Particle::Draw() {
	if (!isAlive_) {
		return;
	}
}

Matrix4x4 Particle::GetWorldMatrix() const
{
	return Matrix4x4::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}
