#include "Collider.h"
#include "ModelManager.h"

void Collider::Initialize() { 
	colliderModel_ = std::make_unique<Object3d>();
	colliderModel_->Initialize();
	ModelManager::GetInstance()->LoadModel("sphereCollider/sphereCollider.obj");
	colliderModel_->SetModel("sphereCollider/sphereCollider.obj");
	colliderModel_->SetRotate({ 0,3.14f,0 });
}

void Collider::UpdateWorldTransform() {
	//ワールド座標をワールドトランスフォームに適用
	colliderModel_->SetTranslate(GetCenterPosition());
	colliderModel_->SetScale({ GetRadius(),GetRadius(),GetRadius()});
	colliderModel_->Update();
}

void Collider::Draw(){
	colliderModel_->Draw();
}