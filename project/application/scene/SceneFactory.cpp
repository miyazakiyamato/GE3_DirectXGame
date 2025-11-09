#include "SceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "FadeInScene.h"
#include "FadeOutScene.h"
#include "DParticleScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	//次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	} else if (sceneName == "GAME") {
		newScene = new GameScene();
	} else if (sceneName == "FADE_IN") {
		newScene = new FadeInScene();
	} else if (sceneName == "FADE_OUT") {
		newScene = new FadeOutScene();
	} else if (sceneName == "D_PARTICLE") {
		newScene = new DParticleScene();
	}
	else {
		assert(0 && "不明なシーン名");
	}
	return newScene;
}
