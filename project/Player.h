#pragma once
#include "BaseCharacter.h"

class Input;
class Camera;
class Player : public BaseCharacter {
public://メンバ関数
	Player() {}

	~Player() {}
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	//ImGui
	void ImGuiDraw();

	//衝突を検知したら呼び出されるコールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;
private://ローカル関数
	void Move();
	void CameraMove();
private://メンバ変数
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;
	//モデルオブジェクト
	std::unique_ptr<Object3d> model_ = nullptr;
	//3Dレティクル
	std::unique_ptr<Object3d> reticle3D_ = nullptr;
	Vector3 velocity_{};
	float moveSpeed_ = 0.5f;

	float hitMovePower = 30.0f;
	Vector3 hitMove_{};

	float destinationAngleX_ = 0.0f;
	float destinationAngleY_ = 0.0f;

	bool isCollision = false;

	int hp_ = 5;

	//攻撃を受けた時
	bool isHit = false;
	const float kColorChangeTime = 5.0f;
	float colorChangeTimer = kColorChangeTime;

public://ゲッターセッター
	int GetHp() { return hp_; }
};

