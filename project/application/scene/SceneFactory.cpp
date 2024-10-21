#include "SceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"


BaseScene* SceneFactory::CreateScene(const std::string& sceveName)
{
	//次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceveName == "TITLE") {
		newScene = new TitleScene();
	}
	else if (sceveName == "GAME") {
		newScene = new GameScene();
	}
	return newScene;
}
