#pragma once
#include <vector>
#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ParticleSystem.h"
#include "CollisionManager.h"
#include "AccelerationField.h"
#include "LevelDataManager.h"

class GameScene : public BaseScene {
public://メンバ関数
	//初期化
	void Initialize() override;
	//終了
	void Finalize() override;
	//毎フレーム更新
	void Update() override;
	//描画
	void Draw() override;
private:
	//衝突判定と応答 
	void CheckAllCollisions();
private://メンバ変数
	//衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;

	bool isAccelerationField = false;
	std::unique_ptr<AccelerationField> accelerationField_ = nullptr;

	std::vector<std::unique_ptr<Object3d>> object3ds_;
	std::vector< std::unique_ptr<Sprite>> sprites_;

	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;
	std::unique_ptr<LevelDataManager> levelDataManager_ = nullptr;
};

