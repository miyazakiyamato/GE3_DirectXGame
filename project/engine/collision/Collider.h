#pragma once
#include "Vector3.h"
#include <Object3d.h>

class Collider {
public:
	virtual ~Collider() = default;
	//初期化
	void Initialize();
	//ワールドトランスフォームの更新
	void UpdateWorldTransform();
	//描画
	void Draw();

	//衝突時に呼ばれる関数
	virtual void OnCollision([[maybe_unused]]Collider* other) {}

private:
	//衝突半径
	float radius_ = 1.5f;
	//識別ID
	uint32_t typeID_ = 0u;
	//コライダーモデル
	std::unique_ptr<Object3d> colliderModel_ = nullptr;

public:
	float GetRadius() { return radius_; }
	void SetRadius(const float& radius) { radius_ = radius; }
	//中心座標を取得
	virtual Vector3 GetCenterPosition() const = 0;
	//識別IDを取得
	uint32_t GetTypeID() const { return typeID_; }
	//識別IDを設定
	void SetTypeID(uint32_t typeID) { typeID_ = typeID; }
};
