#pragma once
#include <memory>
#include "Object3d.h"

class Skydome{
public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
private://メンバ変数
	std::unique_ptr<Object3d> object3d;
};

