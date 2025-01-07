#pragma once
#include "BaseCharacter.h"

class Enemy : public BaseCharacter {
public://メンバ関数
	Enemy() {}

	~Enemy() {}
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

	//衝突を検知したら呼び出されるコールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;
private://メンバ変数
	//モデルオブジェクト
	std::unique_ptr<Object3d> model_ = nullptr;
	int hp_ = 255;

public://ゲッターセッター
	int GetHp() { return hp_; }

	void SetHp(int hp) { hp_ = hp; }
};

