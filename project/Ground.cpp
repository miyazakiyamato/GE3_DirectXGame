#include "Ground.h"

void Ground::Initialize() {
	object3d = std::make_unique<Object3d>();
	object3d->Initialize();
	object3d->SetModel("ground/ground.obj");
	object3d->SetTranslate({ -1,0,0 });
	object3d->SetRotate({ 0,3.14f,0 });
	object3d->SetSubTexture("resources/texture/dirt.png");
}

void Ground::Update() {
	object3d->Update();
}

void Ground::Draw() {
	object3d->Draw();
}