#include "CollisionManager.h"
#include "GlobalVariables.h"

void CollisionManager::Initialize() {
	// imgui
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CollisionManager";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "IsColliderModel", isColliderModel_);
}

void CollisionManager::UpdateWorldTransform() {
	ApplyGlobalVariables();
	//非表示ならぬける
	if (!isColliderModel_) {return;}
	
	//全てのコライダーについて
	for (Collider* collider : colliders_) {
		collider->UpdateWorldTransform();
	}
}

void CollisionManager::Draw() {// 全てのコライダーについて
	// 非表示ならぬける
	if (!isColliderModel_) {return;}

	for (Collider* collider : colliders_) {
		collider->Draw();
	}
}

void CollisionManager::Reset() { colliders_.clear(); }

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	Vector3 positionA = colliderA->GetCenterPosition();
	Vector3 positionB = colliderB->GetCenterPosition();
	//座標差分ベクトル
	Vector3 subtract = positionB - positionA;
	//座標AとBの距離を求める
	float distance = Vector3::Length(subtract);
	//球と球の当たり判定
	if (distance <= (colliderA->GetRadius() + colliderB->GetRadius())) {
		//コライダーの衝突時コールバック
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);
	}
}

void CollisionManager::CheckAllCollisions() {
	//リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;
		//イテレータBはイテレータAの次の要素から回す
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			//ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::AddCollider(Collider* collider) { colliders_.push_back(collider); }

void CollisionManager::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "CollisionManager";
	isColliderModel_ = globalVariables->GetValue<bool>(groupName, "IsColliderModel");
}