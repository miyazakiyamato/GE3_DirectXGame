#pragma once
#include "Framework.h"
#include "Sprite.h"
#include "Object3d.h"

class MyGame : public Framework{
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
};

