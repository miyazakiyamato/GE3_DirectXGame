#include "Collider.h"

void Collider::Initialize() { 
	colliderModel_ = std::make_unique<Object3d>();
	colliderModel_->Initialize();
	colliderModel_->SetModel("cube");
	colliderModel_->SetRotate({ 0,3.14f,0 });
}

void Collider::UpdateWorldTransform() {
	//ワールド座標をワールドトランスフォームに適用
	colliderModel_->SetTranslate(GetCenterPosition());
	colliderModel_->SetScale({ GetRadius() * 2,GetRadius() * 2,GetRadius() * 2 });
	colliderModel_->Update();
}

void Collider::Draw(){
	colliderModel_->Draw();
}