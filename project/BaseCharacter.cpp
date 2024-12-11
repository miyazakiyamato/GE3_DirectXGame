#include "BaseCharacter.h"

void BaseCharacter::Initialize(){
	baseObject3D_ = std::make_unique<Object3d>();
	baseObject3D_->Initialize();
}

void BaseCharacter::Update(){
	baseObject3D_->Update();
}

Vector3 BaseCharacter::GetCenterPosition() const
{
	return baseObject3D_->GetCenterPosition();
}
