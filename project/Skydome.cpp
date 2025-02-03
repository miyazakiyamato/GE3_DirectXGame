#include "Skydome.h"
#include "TextureManager.h"

void Skydome::Initialize(){
	object3d = std::make_unique<Object3d>();
	object3d->Initialize();
	object3d->SetModel("skydome/skydome.obj");
	object3d->SetIsLighting(false);
	object3d->SetTranslate({ -1,0,0 });
	object3d->SetRotate({ 0,3.14f,0 });
	std::string objectName = "skydome";
	TextureManager::GetInstance()->LoadRWTexture("resources/texture/dirt.png", objectName);
	object3d->SetSubTexture("resources/texture/dirt.png", objectName);
}

void Skydome::Update(){
	object3d->Update();
}

void Skydome::Draw(){
	object3d->Draw();
}
