#include "Enemy.h"
#include <imgui.h>
#include "TextureManager.h"

void Enemy::Initialize(){
	BaseCharacter::Initialize();
	Collider::Initialize();
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kEnemy));
	Collider::SetRadius(1.0f);
	baseObject3D_->SetTranslate({ 0.0f,1.0f,0.0f });
	baseObject3D_->SetModel("sphere/sphere.obj");
	baseObject3D_->SetRotate({ 0,3.14f,0 });
	std::string objectName = "enemy";
	TextureManager::GetInstance()->LoadRWTexture("resources/texture/dirt.png", objectName);
	baseObject3D_->SetSubTexture("resources/texture/dirt.png", objectName);
	baseObject3D_->SetIsSubTexture(true);
}

void Enemy::Update(){
	Vector4 color = baseObject3D_->GetColor();
	color.w = (float)hp_ / 255.0f;
	baseObject3D_->SetColor(color);

	BaseCharacter::Update();
}

void Enemy::Draw(){
	baseObject3D_->Draw();
}

void Enemy::ImGuiDraw(){
	std::string groupName = "Enemy";
	if (ImGui::BeginMenu(groupName.c_str())) {
		
		Vector3 translate = baseObject3D_->GetTranslate();
		ImGui::DragFloat3((groupName + ".Transform.Translate").c_str(), &translate.x, 0.1f);

		Vector3 rotate = baseObject3D_->GetRotate();
		ImGui::SliderAngle((groupName + ".Transform.Rotate.x").c_str(), &rotate.x);
		ImGui::SliderAngle((groupName + ".Transform.Rotate.y").c_str(), &rotate.y);
		ImGui::SliderAngle((groupName + ".Transform.Rotate.z").c_str(), &rotate.z);

		Vector3 scale = baseObject3D_->GetScale();
		ImGui::DragFloat3((groupName + ".Transform.Scale").c_str(), &scale.x, 0.1f);

		Vector4 color = baseObject3D_->GetColor();
		ImGui::ColorEdit4((groupName + ".Color").c_str(), &color.x);

		Vector4 highLightColor = baseObject3D_->GetHighLightColor();
		ImGui::ColorEdit4((groupName + ".HighLightColor").c_str(), &highLightColor.x);

		baseObject3D_->SetTranslate(translate);
		baseObject3D_->SetRotate(rotate);
		baseObject3D_->SetScale(scale);
		baseObject3D_->SetColor(color);
		baseObject3D_->SetHighLightColor(highLightColor);

		ImGui::Text("\n");
		ImGui::EndMenu();
	}
}

void Enemy::OnCollision(Collider* other){
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	
}