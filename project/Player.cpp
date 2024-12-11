#include "Player.h"
#include "Input.h"
#include "CameraManager.h"
#include <algorithm>

void Player::Initialize() {
	BaseCharacter::Initialize();
	Collider::Initialize();
	//Collider::SetTypeID(static_cast<uint32_t>());
	Collider::SetRadius(4.0f);
	input_ = Input::GetInstance();
	camera_ = CameraManager::GetInstance()->GetCamera();
	baseObject3D_->SetTranslate({ -1.0f, 1.0f, -30.0f });
	model_ = std::make_unique<Object3d>();
	model_->Initialize();
	model_->SetParent(baseObject3D_.get());
	//model_->SetModel("shere");
	model_->SetRotate({ 0,3.14f,0 });
	reticle3D_ = std::make_unique<Object3d>();
	reticle3D_->Initialize();
	//reticle3D_->SetModel("shere");
	reticle3D_->SetTranslate({ 0,0,10.0f });
	reticle3D_->SetParent(baseObject3D_.get());
}

void Player::Update() {
	BaseCharacter::Update();
	Move();
	CameraMove();
	hitMove_ -= hitMove_ * 0.05f;
	hitMove_ = hitMove_.Clamp01();
	baseObject3D_->SetTranslate(Vector3(baseObject3D_->GetTranslate()) + hitMove_);

	if (isHit) {
		colorChangeTimer -= 1.0f / 60.0f;
		if (colorChangeTimer <= 0.0f) {
			isHit = false;
		}
	}
	
	baseObject3D_->SetTranslate(Vector3::Clamp(baseObject3D_->GetTranslate(), { -100,0,-100 }, { 100,50,100 }));
	reticle3D_->Update();
	model_->Update();
	//isCollision = false;
}

void Player::Draw() {
	//model_->Draw();
	//reticle3D_->Draw();
}

void Player::OnCollision([[maybe_unused]] Collider* other) {
	isCollision = true;

	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	//衝突相手
	
}

void Player::Move() {// 移動量
	velocity_ = { 0,0,0 };
	if (input_->PushKey(DIK_A)) {
		velocity_.x -= moveSpeed_;
	}
	if (input_->PushKey(DIK_D)) {
		velocity_.x += moveSpeed_;
	}
	if (input_->PushKey(DIK_W)) {
		velocity_.z += moveSpeed_;
	}
	if (input_->PushKey(DIK_S)) {
		velocity_.z -= moveSpeed_;
	}
	if (velocity_.Length() != 0) {
		velocity_.Normalize();
		velocity_ *= moveSpeed_;

		Matrix4x4 rotateMatrix = Matrix4x4::MakeRotateYMatrix(camera_->GetRotate().y);
		velocity_ = Matrix4x4::Transform(velocity_, rotateMatrix);

		Vector3 rotation{};
		rotation.y = std::atan2f(velocity_.x, velocity_.z);
		Vector3 velocityZ = Matrix4x4::Transform(velocity_, Matrix4x4::MakeRotateYMatrix(-rotation.y));
		rotation.x = std::atan2f(-velocityZ.y, velocityZ.z);

		Vector3 position = baseObject3D_->GetTranslate();
		baseObject3D_->SetTranslate(position + velocity_);
		baseObject3D_->SetRotate(rotation);
	}
}

void Player::CameraMove() {
	// 速さ
	const float speed = 0.1f;
	// 移動量
	Vector3 move{};
	// 移動
	Vector3 rotate = camera_->GetRotate();

	if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		move += { 0.0f, -1.0f, 0.0f };
	}
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		move += { 0.0f, 1.0f, 0.0f };
	}
	if (Input::GetInstance()->PushKey(DIK_UP)) {
		move += { -1.0f, 0.0f, 0.0f };
	}
	if (Input::GetInstance()->PushKey(DIK_DOWN)) {
		move += { 1.0f, 0.0f, 0.0f };
	}

	if (move.Length() != 0) {
		move.Normalize();
		move *= speed;

		destinationAngleX_ = std::clamp(rotate.x + move.x, -1.5f, 1.5f);
		destinationAngleY_ = rotate.y + move.y;
	}

	rotate.x = rotate.x + 0.1f * (destinationAngleX_ - rotate.x);
	rotate.y = rotate.y + 0.1f * (destinationAngleY_ - rotate.y);
	camera_->SetRotate(rotate);
	//追従対象からのオフセット
	Vector3 offset = { 0.0f, 0.0f, -10.0f };
	//回転行列の合成
	Matrix4x4 rotateMatrix = Matrix4x4::MakeAffineMatrix({ 1, 1, 1 }, camera_->GetRotate(), {});
	//オフセットをカメラの回転に合わせて回転させる
	offset = Matrix4x4::TransformNormal(offset, rotateMatrix);
	Vector3 position = baseObject3D_->GetTranslate();
	camera_->SetTranslate(position + offset);

	// Y軸周り角度
	Vector3 rotation = baseObject3D_->GetRotate();
	rotation.y = camera_->GetRotate().y;
	baseObject3D_->SetRotate(rotation);
	baseObject3D_->SetRotate(camera_->GetRotate());
}
