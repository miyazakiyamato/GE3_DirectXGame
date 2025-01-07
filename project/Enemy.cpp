#include "Enemy.h"

void Enemy::Initialize(){
	BaseCharacter::Initialize();
	Collider::Initialize();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
	Collider::SetRadius(1.0f);
	baseObject3D_->SetTranslate({ 0.0f,1.0f,0.0f });
	model_ = std::make_unique<Object3d>();
	model_->Initialize();
	model_->SetParent(baseObject3D_.get());
	model_->SetModel("sphere");
	model_->SetRotate({ 0,3.14f,0 });
	model_->SetTexture("resources/dirt.png");
}

void Enemy::Update(){
	BaseCharacter::Update();
	Vector4 color = model_->GetColor();
	color.w = (float)hp_ / 255.0f;
	model_->SetColor(color);
	model_->Update();
}

void Enemy::Draw(){
	model_->Draw();
}

void Enemy::OnCollision(Collider* other){
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	
}