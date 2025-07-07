#include "GameScene.h"
#include <imgui.h>
#include "Input.h"
#include "CameraManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "ParticleManager.h"
#include "GlobalVariables.h"
#include "TimeManager.h"
#include "Line3D.h"
#include "HitEffect.h"

void GameScene::Initialize(){
	BaseScene::Initialize();

	//開発用のUIの処理。
	// ウインドウのサイズを固定する
	ImGui::SetNextWindowSize(ImVec2(1280, 40));
	// ウインドウの位置を設定する
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	CameraManager::GetInstance()->SetCamera("Camera2");
	CameraManager::GetInstance()->FindCamera("Camera2");
	CameraManager::GetInstance()->GetCamera()->SetRotate({ 0.3f,-0.8f,0.0f });
	CameraManager::GetInstance()->GetCamera()->SetTranslate({ 8.0f,4.0f,-8.0f });

	CameraManager::GetInstance()->FindCamera("default");

	AudioManager::GetInstance()->LoadWave("maou_se_system48.wav");
	//AudioManager::GetInstance()->LoadMP3("audiostock_1420737.mp3");

	/*ModelManager::GetInstance()->LoadModel("plane/plane.obj");
	ModelManager::GetInstance()->LoadModel("fence/fence.obj");
	ModelManager::GetInstance()->LoadModel("axis/axis.obj");
	ModelManager::GetInstance()->LoadModel("sphere/sphere.obj");*/
	ModelManager::GetInstance()->LoadModel("terrain/terrain.obj");
	ModelManager::GetInstance()->LoadModel("skybox");
	ModelManager::GetInstance()->LoadModel("ground/ground.obj");
	ModelManager::GetInstance()->LoadModel("multiMesh/multiMesh.obj");
	/*ModelManager::GetInstance()->LoadModel("plane/plane.gltf");*/

	ModelManager::GetInstance()->LoadModel("AnimatedCube/AnimatedCube.gltf");
	ModelManager::GetInstance()->LoadAnimation("AnimatedCube/AnimatedCube.gltf");
	ModelManager::GetInstance()->LoadModel("simpleSkin/simpleSkin.gltf");
	ModelManager::GetInstance()->LoadAnimation("simpleSkin/simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("human/sneakWalk.gltf");
	ModelManager::GetInstance()->LoadAnimation("human/sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("human/walk.gltf");
	ModelManager::GetInstance()->LoadAnimation("human/walk.gltf");
	ModelManager::GetInstance()->LoadModel("BrainStem/BrainStem.gltf");
	ModelManager::GetInstance()->LoadAnimation("BrainStem/BrainStem.gltf");

	TextureManager::GetInstance()->LoadTexture("circle2.png");
	TextureManager::GetInstance()->LoadTexture("gradationLine.png");
	TextureManager::GetInstance()->LoadTexture("rostock_laage_airport_4k.dds");

	//衝突マネージャの生成
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize();

	//skybox
	std::unique_ptr<Object3d> object3d(new Object3d);
	object3d->Initialize();
	object3d->SetScale({ 1000.0f,1000.0f,1000.0f });
	object3d->SetModel("skybox");
	object3d->SetTexture("rostock_laage_airport_4k.dds");
	object3ds_.push_back(std::move(object3d));
	std::unique_ptr<Object3d> object3d2(new Object3d);
	object3d2->Initialize();
	object3d2->SetTranslate({-1.0f,0.0f,0.0f});
	object3d2->SetModel("BrainStem/BrainStem.gltf");
	object3d2->SetAnimation("BrainStem/BrainStem.gltf", true);
	//object3d2->SetEnvironmentTexture("rostock_laage_airport_4k.dds");
	object3ds_.push_back(std::move(object3d2));
	//レベルデータマネージャの生成
	levelDataManager_ = std::make_unique<LevelDataManager>();
	//レベルデータの読み込み取得
	levelDataManager_->LoadJsonFile("level1"); 
	LevelDataManager::LevelData* levelData = levelDataManager_->GetObjectData("level1");
	for (const std::unique_ptr<ObjectData>& objectData : *levelData) {
		if (objectData->typeName == "MESH") {
			std::unique_ptr<Object3d> object3d(new Object3d);
			object3d->Initialize();
			object3d->SetScale(objectData->scaling);
			object3d->SetRotate(objectData->rotation);
			object3d->SetTranslate(objectData->translation);
			if (!objectData->fileName.empty()) {
				//object3d->SetModel(objectData->fileName);
				object3d->SetModel("multiMesh/multiMesh.obj");
				object3d->SetEnvironmentTexture("rostock_laage_airport_4k.dds");
			}
			object3ds_.push_back(std::move(object3d));
		}
		if (objectData->typeName == "ARMATURE") {
			for (const std::unique_ptr<ObjectData>&childData : objectData->children) {
				if (childData->typeName == "MESH") {
					std::unique_ptr<Object3d> object3d(new Object3d);
					object3d->Initialize();
					object3d->SetTranslate(objectData->translation);
					if (!childData->fileName.empty()) {
						object3d->SetModel(childData->fileName);
						object3d->SetAnimation(childData->fileName, true);
						object3d->SetEnvironmentTexture("rostock_laage_airport_4k.dds");
					}
					object3ds_.push_back(std::move(object3d));
				}
			}
		}
	}
	

	/*object3ds_[0]->SetModel("AnimatedCube/AnimatedCube.gltf");
	object3ds_[0]->SetAnimation("AnimatedCube/AnimatedCube.gltf",true);*/
	/*object3ds_[0]->SetModel("simpleSkin/simpleSkin.gltf");
	object3ds_[0]->SetAnimation("simpleSkin/simpleSkin.gltf",true);*/
	/*object3ds_[1]->SetTranslate({ -1,0,0 });
	object3ds_[1]->SetRotate({ 0,3.14f,0 });
	object3ds_[1]->SetModel("terrain/terrain.obj");*/
	//object3ds_[1]->SetModel("plane/plane.gltf");
	//object3ds_[1]->SetModel("axis/axis.obj");
	/*object3ds_[1]->SetModel("human/sneakWalk.gltf");
	object3ds_[1]->SetAnimation("human/sneakWalk.gltf", true);
	object3ds_[1]->SetTranslate({ 1,0,0 });
	object3ds_[1]->SetRotate({ 0,3.14f,0 });
	object3ds_[2]->SetModel("human/walk.gltf");
	object3ds_[2]->SetAnimation("human/walk.gltf", true);
	object3ds_[2]->SetTranslate({ 0,0,0 });
	object3ds_[2]->SetRotate({ 0,3.14f,0 });*/
	//
	isAccelerationField = false;
	accelerationField_.reset(new AccelerationField);

	//ParticleManager::GetInstance()->CreateParticleGroup();
	particleSystem_.reset(new ParticleSystem);
	std::unique_ptr<BaseParticleEmitter> hitEffect = std::make_unique<HitEffect>();
	particleSystem_->CreateParticleEmitter("hitEffect", std::move(hitEffect));
	
	//スプライトの初期化
	for (uint32_t i = 0; i < 5; ++i) {
		std::unique_ptr<Sprite> sprite(new Sprite);
		sprite->Initialize("uvChecker.png");
		sprite->SetPosition({ 100 + 200.0f * float(i), 100 });
		sprite->SetSize({ 100.0f,100.0f });
		sprites_.push_back(std::move(sprite));
	}
	sprites_[0]->SetTextureSize({ 64.0f,64.0f });
	sprites_[1]->SetTexture("monsterBall.png");
	sprites_[1]->SetIsFlipX(true);
	sprites_[2]->SetIsFlipY(true);
	sprites_[3]->SetIsFlipX(true);
	sprites_[3]->SetIsFlipY(true);
}

void GameScene::Finalize(){
	//解放
	for (std::unique_ptr<Object3d>& object3d : object3ds_) {
		object3d.reset();  // メモリを解放する
	}

	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite.reset();  // メモリを解放する
	}
	BaseScene::Finalize();
}

void GameScene::Update(){
	BaseScene::Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		AudioManager::GetInstance()->PlayWave("maou_se_system48.wav");
		//AudioManager::GetInstance()->PlayMP3("audiostock_1420737.mp3");
		//ParticleManager::GetInstance()->Emit("uvChecker", { 0,0,0 }, 10);
		particleSystem_->Emit("hitEffect");
	}

#ifdef _DEBUG
	//// ウインドウフラグに NoResize を指定
	//ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoResize);
	//ImGui::ShowDemoWindow();
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	globalVariables->Update();
	std::string groupName = "";
	if (ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar()) {
			CameraManager::GetInstance()->ImGuiUpdate();

			LightManager::GetInstance()->ImGuiUpdate();
			
			size_t object3dCount = 0;
			for (std::unique_ptr<Object3d>& object3d : object3ds_) {
				std::string objectName = ("Object3d" + std::to_string(object3dCount)).c_str();
				object3d->ImGuiUpdate(objectName);

				object3dCount++;
			}
			ParticleManager::GetInstance()->UpdateGlobalVariables();
			groupName = "Particle";
			
			groupName = "Sprite";
			uint32_t spriteIDIndex = 0;
			for (std::unique_ptr<Sprite>& sprite : sprites_) {
				std::string spriteName = ("Sprite" + std::to_string(spriteIDIndex)).c_str();
				sprite->ImGuiUpdate(spriteName);
				++spriteIDIndex;
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
#endif //_DEBUG
	CameraManager::GetInstance()->GetCamera()->Update();

#ifdef _DEBUG
	// デバッグ用にワールドトランスフォームの更新
	collisionManager_->UpdateWorldTransform();
#endif //_DEBUG

	for (std::unique_ptr<Object3d>& object3d : object3ds_) {
		object3d->Update();
	}

	if (isAccelerationField) {
		for (std::pair<const std::string, std::unique_ptr<ParticleManager::ParticleGroup>>& pair : ParticleManager::GetInstance()->GetParticleGroups()) {
			ParticleManager::ParticleGroup& group = *pair.second;
			int index = 0;
			for (std::list<ParticleManager::Particle>::iterator it = group.particles.begin(); it != group.particles.end();) {
				ParticleManager::Particle& particle = *it;

				if (Collision::IsCollision(accelerationField_->GetAABB(), particle.transform.translate)) {
					particle.velocity += accelerationField_->GetAcceleration() * TimeManager::GetInstance()->deltaTime_;

				}

				++it;
				++index;
			}
		}
	}

	particleSystem_->Update();

	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Update();
	}
}

void GameScene::Draw(){
	//Object3dの描画
	for (std::unique_ptr<Object3d>& object3d : object3ds_) {
		object3d->Draw();
	}
	//object3ds_[0]->Draw();
	//当たり判定の表示
	collisionManager_->Draw();
	
	//ラインの描画
	//Line3dManager::GetInstance()->DrawLine(object3ds_[0]->GetCenterPosition(), object3ds_[1]->GetCenterPosition(),{1.0f,0.0f,0.0f,1.0f});
	//Line3dManager::GetInstance()->DrawLine(object3ds_[1]->GetCenterPosition(), object3ds_[2]->GetCenterPosition(),{1.0f,0.0f,0.0f,1.0f});
	//Line3dManager::GetInstance()->DrawSphere({ object3ds_[0]->GetCenterPosition(),1.0f }, { 1.0f,0.0f,0.0f,1.0f });
	/*Line3dManager::GetInstance()->DrawSphere({ {},1.0f}, {1.0f,0.0f,0.0f,1.0f},10);
	Line3dManager::GetInstance()->DrawGrid({50.0f,3.0f});*/
	Line3dManager::GetInstance()->Draw();

	//Particleの描画
	particleSystem_->Draw();

	//Spriteの描画
	for (std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Draw();
	}
}

void GameScene::CheckAllCollisions(){
	//衝突マネージャのリストクリアする
	collisionManager_->Reset();
	//全てのコライダーを衝突マネージャのリストに登録する

	/*for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		collisionManager_->AddCollider(enemy.get());
	}*/
	//リスト内の総当たり判定
	collisionManager_->CheckAllCollisions();
}
