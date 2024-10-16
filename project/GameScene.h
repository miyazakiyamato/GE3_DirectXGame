#pragma once
#include <vector>
#include "Sprite.h"
#include "Object3d.h"

class Input;
class GameScene{
public://メンバ関数
	//初期化
	void Initialize();
	//終了
	void Finalize();
	//毎フレーム更新
	void Update();
	//描画
	void Draw();
private://メンバ変数
	Input* input_ = nullptr;

	std::vector<Object3d*> object3ds;
	std::vector<Sprite*> sprites;
	/*std::vector<std::unique_ptr<Object3d>> object3ds_;
	std::vector< std::unique_ptr<Sprite>> sprites_;*/
};

