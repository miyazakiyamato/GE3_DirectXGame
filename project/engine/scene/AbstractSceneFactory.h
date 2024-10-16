#pragma once
#include <string>
#include "BaseScene.h"

/// <summary>
/// シーン工場
/// </summary>
class AbstractSceneFactory{
public:
	//仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;
	//シーン生成
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;
};

