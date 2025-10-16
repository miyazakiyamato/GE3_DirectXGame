#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include <vector>

class TitleScene : public BaseScene {
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
	std::vector< std::unique_ptr<Sprite>> sprites_;
};

