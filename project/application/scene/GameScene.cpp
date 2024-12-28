#include "GameScene.h"
#include <imgui.h>
#include "Input.h"
#include "CameraManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "ParticleManager.h"

void GameScene::Initialize(){
	BaseScene::Initialize();

	//開発用のUIの処理。
	// ウインドウのサイズを固定する
	ImGui::SetNextWindowSize(ImVec2(500, 300));
	// ウインドウの位置を設定する
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	CameraManager::GetInstance()->SetCamera("Camera2");
	CameraManager::GetInstance()->FindCamera("Camera2");
	CameraManager::GetInstance()->GetCamera()->SetRotate({ 0.3f,-0.8f,0.0f });
	CameraManager::GetInstance()->GetCamera()->SetTranslate({ 8.0f,4.0f,-8.0f });

	CameraManager::GetInstance()->FindCamera("default");

	AudioManager::GetInstance()->LoadWave("maou_se_system48.wav");
	//AudioManager::GetInstance()->LoadMP3("audiostock_1420737.mp3");

	//衝突マネージャの生成
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize();

	for (uint32_t i = 0; i < 3; ++i) {
		Object3d* object3d = new Object3d;
		object3d->Initialize();
		object3ds.push_back(object3d);
	}

	ModelManager::GetInstance()->LoadModel("plane");
	ModelManager::GetInstance()->LoadModel("fence");
	ModelManager::GetInstance()->LoadModel("axis");
	ModelManager::GetInstance()->LoadModel("ground");
	ModelManager::GetInstance()->LoadModel("skydome");
	ModelManager::GetInstance()->LoadModel("sphereCollider");

	object3ds[0]->SetModel("plane");
	object3ds[0]->SetTranslate({ -1,0,0 });
	object3ds[0]->SetRotate({ 0,3.14f,0 });
	object3ds[1]->SetModel("axis");
	object3ds[1]->SetTranslate({ 1,0,0 });
	object3ds[1]->SetRotate({ 0,3.14f,0 });
	object3ds[2]->SetModel("plane");
	object3ds[2]->SetTranslate({ -2,0,0 });
	object3ds[2]->SetRotate({ 0,3.14f,0 });
	
	skydome_ = new Skydome;
	skydome_->Initialize();

	ground_ = new Ground();
	ground_->Initialize();

	player_ = new Player;
	player_->Initialize();
	//
	isAccelerationField = false;
	accelerationField_ = new AccelerationField;

	//ParticleManager::GetInstance()->CreateParticleGroup();
	particleEmitter_ = new ParticleEmitter();
	particleEmitter_->Initialize("circle", "resources/circle.png");

	//スプライトの初期化
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite;
		sprite->Initialize("resources/uvChecker.png");
		sprite->SetPosition({ 100 + 200.0f * float(i), 100 });
		sprite->SetSize({ 100.0f,100.0f });
		sprites.push_back(sprite);
	}
	sprites[0]->SetTextureSize({ 64.0f,64.0f });
	sprites[1]->SetTexture("resources/monsterBall.png");
	sprites[1]->SetIsFlipX(true);
	sprites[2]->SetIsFlipY(true);
	sprites[3]->SetIsFlipX(true);
	sprites[3]->SetIsFlipY(true);
}

void GameScene::Finalize(){
	//解放
	delete particleEmitter_;
	delete accelerationField_;
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	delete player_;
	delete ground_;
	delete skydome_;
	for (Object3d* object3d : object3ds) {
		delete object3d;
	}
	BaseScene::Finalize();
}

void GameScene::Update(){
	BaseScene::Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		AudioManager::GetInstance()->PlayWave("maou_se_system48.wav");
		//AudioManager::GetInstance()->PlayMP3("audiostock_1420737.mp3");
		//ParticleManager::GetInstance()->Emit("uvChecker", { 0,0,0 }, 10);
		particleEmitter_->Emit();
	}

#ifdef _DEBUG
	//// ウインドウフラグに NoResize を指定
	//ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoResize);
	ImGui::Begin("Settings");
	//ImGui::ShowDemoWindow();

	if (ImGui::CollapsingHeader("Camera"))
	{
		static ImGuiComboFlags cameraFlags = 0;
		const char* items[] = { "default","Camera2" };
		static int cameraItem_selected_idx = 0; // Here we store our selection data as an index.

		// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
		const char* combo_preview_value = items[cameraItem_selected_idx];

		if (ImGui::BeginCombo("Now Camera", combo_preview_value, cameraFlags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (cameraItem_selected_idx == n);
				if (ImGui::Selectable(items[n], is_selected)) {
					cameraItem_selected_idx = n;
					CameraManager::GetInstance()->FindCamera(items[n]);
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		Vector3 cameraRotate = CameraManager::GetInstance()->GetCamera()->GetRotate();
		ImGui::DragFloat3("Camera.Rotate", &cameraRotate.x, 0.001f);
		CameraManager::GetInstance()->GetCamera()->SetRotate(cameraRotate);

		Vector3 cameraPosition = CameraManager::GetInstance()->GetCamera()->GetTranslate();
		ImGui::DragFloat3("Camera.Translate", &cameraPosition.x, 0.01f);
		CameraManager::GetInstance()->GetCamera()->SetTranslate(cameraPosition);

		ImGui::Text("\n");
	}
	//if (ImGui::CollapsingHeader("Light"))
	//{
	//	static ImGuiComboFlags lightFlags = 0;
	//	const char* items[] = { "default"/*,"Light2"*/ };
	//	static int lightItem_selected_idx = 0; // Here we store our selection data as an index.

	//	// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
	//	const char* combo_preview_value = items[lightItem_selected_idx];

	//	if (ImGui::BeginCombo("Now Light", combo_preview_value, lightFlags))
	//	{
	//		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
	//		{
	//			const bool is_selected = (lightItem_selected_idx == n);
	//			if (ImGui::Selectable(items[n], is_selected)) {
	//				lightItem_selected_idx = n;
	//				LightManager::GetInstance()->FindLight(items[n]);
	//			}

	//			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
	//			if (is_selected) {
	//				ImGui::SetItemDefaultFocus();
	//			}
	//		}
	//		ImGui::EndCombo();
	//	}
	//	
	//	Vector4 lightColor = LightManager::GetInstance()->GetLight()->GetColor();
	//	ImGui::ColorEdit4("Light.Color", &lightColor.x);
	//	LightManager::GetInstance()->GetLight()->SetColor(lightColor);

	//	Vector3 lightDirection = LightManager::GetInstance()->GetLight()->GetDirection();
	//	ImGui::DragFloat3("Light.Direction", &lightDirection.x, 0.01f);
	//	lightDirection = lightDirection.Normalize();
	//	LightManager::GetInstance()->GetLight()->SetDirection(lightDirection);

	//	float lightIntensity = LightManager::GetInstance()->GetLight()->GetIntensity();
	//	ImGui::DragFloat("Light.Intensity", &lightIntensity, 0.01f);
	//	LightManager::GetInstance()->GetLight()->SetIntensity(lightIntensity);

	//	ImGui::Text("\n");
	//}
	if (ImGui::CollapsingHeader("Object3d"))
	{
		static ImGuiComboFlags Object3dFlags = 0;
		const char* items[] = {
		"None",      //!< ブレンドなし
		"Normal",    //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
		"Add",       //!< 加算。Src * SrcA + Dest * 1
		"Subtract",  //!< 減算。Dest * 1 - Src * SrcA
		"Multiply",  //!< 乗算。Src * 0 + Dest * Src
		"Screen",};
		static int Object3dItem_selected_idx = 1; // Here we store our selection data as an index.

		// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
		const char* combo_preview_value = items[Object3dItem_selected_idx];

		if (ImGui::BeginCombo("Now Blend", combo_preview_value, Object3dFlags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (Object3dItem_selected_idx == n);
				if (ImGui::Selectable(items[n], is_selected)) {
					Object3dItem_selected_idx = n;
					ModelManager::GetInstance()->ChangeBlendMode(static_cast<ModelCommon::BlendMode>(n));
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		size_t object3dCount = 0;
		for (Object3d* object3d : object3ds) {
			Vector3 translate = object3d->GetTranslate();
			ImGui::DragFloat3(("Object3d " + std::to_string(object3dCount) + ".Transform.Translate").c_str(), &translate.x, 0.1f);
			object3d->SetTranslate(translate);

			Vector3 rotate = object3d->GetRotate();
			ImGui::SliderAngle(("Object3d " + std::to_string(object3dCount) + ".Transform.Rotate.x").c_str(), &rotate.x);
			ImGui::SliderAngle(("Object3d " + std::to_string(object3dCount) + ".Transform.Rotate.y").c_str(), &rotate.y);
			ImGui::SliderAngle(("Object3d " + std::to_string(object3dCount) + ".Transform.Rotate.z").c_str(), &rotate.z);
			object3d->SetRotate(rotate);

			Vector3 scale = object3d->GetScale();
			ImGui::DragFloat3(("Object3d " + std::to_string(object3dCount) + ".Transform.Scale").c_str(), &scale.x, 0.1f);
			object3d->SetScale(scale);

			Vector4 color = object3d->GetColor();
			ImGui::ColorEdit4(("Object3d " + std::to_string(object3dCount) + ".Color").c_str(), &color.x);
			object3d->SetColor(color);

			ImGui::Text("\n");

			object3dCount++;
		}
	}
	if (ImGui::CollapsingHeader("particle")) {
		static ImGuiComboFlags particleFlags = 0;
		const char* items[] = {
		"None",      //!< ブレンドなし
		"Normal",    //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
		"Add",       //!< 加算。Src * SrcA + Dest * 1
		"Subtract",  //!< 減算。Dest * 1 - Src * SrcA
		"Multiply",  //!< 乗算。Src * 0 + Dest * Src
		"Screen", };
		static int particleItem_selected_idx = 2; // Here we store our selection data as an index.

		// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
		const char* combo_preview_value = items[particleItem_selected_idx];

		if (ImGui::BeginCombo("Now Blend", combo_preview_value, particleFlags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (particleItem_selected_idx == n);
				if (ImGui::Selectable(items[n], is_selected)) {
					particleItem_selected_idx = n;
					ParticleManager::GetInstance()->ChangeBlendMode(static_cast<ParticleCommon::BlendMode>(n));
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		/*size_t spriteCount = 0;
		for (ParticleEmitter* particle : sprites) {*/
			Vector3 position = particleEmitter_->GetPosition();
			ImGui::DragFloat2("particleEmitter_.Translate", &position.x, 0.1f);
			/*if (position.y > 640.0f) {
				position.y = 640.0f;
			}*/
			particleEmitter_->SetPosition(position);

			/*Vector3 rotation = particleEmitter_->GetRotation();
			ImGui::SliderAngle("particleEmitter_.Rotate", &rotation.x);
			particleEmitter_->SetRotation(rotation);

			Vector3 size = particleEmitter_->GetSize();
			ImGui::DragFloat2("particleEmitter_.Scale", &size.x, 0.1f);
			if (size.y > 360.0f) {
				size.y = 360.0f;
			}
			particleEmitter_->SetSize(size);*/

			int count = particleEmitter_->GetCount();
			ImGui::DragInt("particleEmitter_.count", &count,1,0,1000);
			particleEmitter_->SetCount(count);

			float frequency = particleEmitter_->GetFrequency();
			ImGui::DragFloat("particleEmitter_.frequency", &frequency, 0.1f);
			particleEmitter_->SetFrequency(frequency);

			if (ImGui::Button("ParticleEmit", { 100,50 })) {
				particleEmitter_->Emit();
			}

			bool isEmitUpdate = particleEmitter_->GetIsEmitUpdate();
			ImGui::Checkbox("IsEmitUpdate", &isEmitUpdate);
			particleEmitter_->SetIsEmitUpdate(isEmitUpdate);
			
			ImGui::Checkbox("IsAccelerationField", &isAccelerationField);

			ImGui::Text("\n");
			
		//}
	}
	if (ImGui::CollapsingHeader("Sprite")) {
		static ImGuiComboFlags spriteFlags = 0;
		const char* items[] = {
		"None",      //!< ブレンドなし
		"Normal",    //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
		"Add",       //!< 加算。Src * SrcA + Dest * 1
		"Subtract",  //!< 減算。Dest * 1 - Src * SrcA
		"Multiply",  //!< 乗算。Src * 0 + Dest * Src
		"Screen", };
		static int spriteItem_selected_idx = 1; // Here we store our selection data as an index.

		// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
		const char* combo_preview_value = items[spriteItem_selected_idx];

		if (ImGui::BeginCombo("Now Blend", combo_preview_value, spriteFlags))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (spriteItem_selected_idx == n);
				if (ImGui::Selectable(items[n], is_selected)) {
					spriteItem_selected_idx = n;
					TextureManager::GetInstance()->ChangeBlendMode(static_cast<SpriteCommon::BlendMode>(n));
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		size_t spriteCount = 0;
		for (Sprite* sprite : sprites) {
			Vector2 position = sprite->GetPosition();
			ImGui::DragFloat2(("Sprite " + std::to_string(spriteCount) + ".Translate").c_str(), &position.x, 1.0f, 0.0f, 1180.0f, "%.1f");
			/*if (position.y > 640.0f) {
				position.y = 640.0f;
			}*/
			sprite->SetPosition(position);

			float rotation = sprite->GetRotation();
			ImGui::SliderAngle(("Sprite " + std::to_string(spriteCount) + ".Rotate").c_str(), &rotation);
			sprite->SetRotation(rotation);

			Vector2 size = sprite->GetSize();
			ImGui::DragFloat2(("Sprite " + std::to_string(spriteCount) + ".Scale").c_str(), &size.x, 1.0f, 0.0f, 640.0f, "%.1f");
			if (size.y > 360.0f) {
				size.y = 360.0f;
			}
			sprite->SetSize(size);

			Vector4 color = sprite->GetColor();
			ImGui::ColorEdit4(("Sprite " + std::to_string(spriteCount) + ".Color").c_str(), &color.x);
			sprite->SetColor(color);

			ImGui::Text("\n");
			spriteCount++;
		}
	}
	//ImGui::Checkbox("useMonsterBall", &useMonsterBall);
	//ImGui::ColorEdit4("DirectionalLightData.Color", &directionalLightData->color.x);
	//ImGui::DragFloat3("DirectionalLightData.Direction", &directionalLightData->direction.x, 0.01f, -1.0f, 1.0f);
	//directionalLightData->direction = Vector3::Normalize(directionalLightData->direction);
	//ImGui::DragFloat("DirectionalLightData.Intensity", &directionalLightData->intensity, 0.01f, 0.0f, 1.0f);
	/*ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);*/
	ImGui::End();

	// デバッグ用にワールドトランスフォームの更新
	collisionManager_->UpdateWorldTransform();
#endif //_DEBUG

	for (Object3d* object3d : object3ds) {
		object3d->Update();
	}
	skydome_->Update();
	ground_->Update();

	player_->Update();
	if (isAccelerationField) {
		for (std::pair<const std::string, std::unique_ptr<ParticleManager::ParticleGroup>>& pair : ParticleManager::GetInstance()->GetParticleGroups()) {
			ParticleManager::ParticleGroup& group = *pair.second;
			int index = 0;
			for (std::list<ParticleManager::Particle>::iterator it = group.particles.begin(); it != group.particles.end();) {
				ParticleManager::Particle& particle = *it;

				if (Collision::IsCollision(accelerationField_->GetAABB(), particle.transform.translate)) {
					particle.velocity += accelerationField_->GetAcceleration() * kDeltaTime_;

				}

				++it;
				++index;
			}
		}
	}
	particleEmitter_->Update();
	ParticleManager::GetInstance()->Update();

	for (Sprite* sprite : sprites) {
		sprite->Update();
	}
}

void GameScene::Draw(){
	//Modelの描画準備Modelの描画に共通グラフィックコマンドを積む
	ModelManager::GetInstance()->DrawCommonSetting();
	skydome_->Draw();
	ground_->Draw();
	
	for (Object3d* object3d : object3ds) {
		object3d->Draw();
	}
	player_->Draw();
	//当たり判定の表示
	collisionManager_->Draw();
	//Particleの描画準備Modelの描画に共通グラフィックコマンドを積む
	ParticleManager::GetInstance()->Draw();

	//Spriteの描画準備Spriteの描画に共通のグラフィックコマンドを積む
	TextureManager::GetInstance()->DrawCommonSetting();
	/*for (Sprite* sprite : sprites) {
		sprite->Draw();
	}*/
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
