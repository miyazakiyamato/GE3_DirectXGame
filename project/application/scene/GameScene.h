#pragma once
#include <vector>
#include "BaseScene.h"
#include "Sprite.h"
#include "Object3d.h"

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
private://メンバ変数
	std::vector<Object3d*> object3ds;
	std::vector<Sprite*> sprites;
	/*std::vector<std::unique_ptr<Object3d>> object3ds_;
	std::vector< std::unique_ptr<Sprite>> sprites_;*/
};

