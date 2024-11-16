#include "Particle.h"
#include "ParticleManager.h"
#include "CameraManager.h"
#include "TextureManager.h"


void Particle::Initialize(std::string textureFilePath) {
	textureFilePath_ = textureFilePath;

	//画像サイズをテクスチャサイズに合わせる
	AdjustTextureSize();
	
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
	
}

void Particle::AdjustTextureSize(){
	//テクスチャデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath_);
	//画像サイズをテクスチャサイズに合わせる
	/*textureSize_.x = static_cast<float>(metadata.width);
	textureSize_.y = static_cast<float>(metadata.height);*/
}

Matrix4x4 Particle::GetWorldMatrix() const
{
	return Matrix4x4::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
}
