//#pragma warning(push)
//#pragma warning(disable:4023)
//#pragma warning(pop)
#define _USE_MATH_DEFINES
#include <dxgidebug.h>
#include <cmath>
#include <vector>
#include <imgui.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"
#include "Camera.h"
#include "CameraManager.h"
#include "SrvManager.h"
#include "ImGuiManager.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	(void)CoInitializeEx(0, COINIT_MULTITHREADED);
	D3DResourceLeakChecker leakCheck;

	//ポインタ
	WinApp* winApp = nullptr;
	Input* input = nullptr;
	DirectXCommon* dxCommon = nullptr;
	SrvManager* srvManager = nullptr;
	ImGuiManager* imGuiManager = nullptr;
	std::vector<Object3d*> object3ds;
	SpriteCommon* spriteCommon = nullptr;
	std::vector<Sprite*> sprites;
	

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	//DirecXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//入力の初期化
	input = new Input();
	input->Initialize(winApp);
	
	srvManager = new SrvManager();
	srvManager->Initialize(dxCommon);

	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize(winApp,dxCommon,srvManager);

	//テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon,srvManager);

	ModelManager::GetInstance()->Initialize(dxCommon);

	CameraManager::GetInstance()->Initialize();

	CameraManager::GetInstance()->SetCamera("default");
	CameraManager::GetInstance()->SetCamera("Camera2");

	CameraManager::GetInstance()->FindCamera("default");
	CameraManager::GetInstance()->GetCamera()->SetRotate({0.3f,0.0f,0.0f});
	CameraManager::GetInstance()->GetCamera()->SetTranslate({ 0.0f,4.0f,-10.0f });
	CameraManager::GetInstance()->FindCamera("Camera2");
	CameraManager::GetInstance()->GetCamera()->SetRotate({ 0.3f,-0.8f,0.0f });
	CameraManager::GetInstance()->GetCamera()->SetTranslate({ 8.0f,4.0f,-8.0f });
	
	CameraManager::GetInstance()->FindCamera("default");

	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	for (uint32_t i = 0; i < 2; ++i) {
		Object3d* object3d = new Object3d;
		object3d->Initialize();
		object3ds.push_back(object3d);
	}
	object3ds[0]->SetModel("plane.obj");
	object3ds[0]->SetTranslate({-2,0,0});
	object3ds[0]->SetRotate({0,3.14f,0});
	object3ds[1]->SetModel("axis.obj");
	object3ds[1]->SetTranslate({3,0,0});
	//スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

	//スプライトの初期化
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite;
		sprite->Initialize(spriteCommon,"resources/uvChecker.png");
		sprite->SetPosition({100 + 200.0f * float(i), 100});
		sprite->SetSize({ 100.0f,100.0f });
		sprites.push_back(sprite);
	}
	sprites[0]->SetTextureSize({64.0f,64.0f});
	sprites[1]->SetTexture("resources/monsterBall.png");
	sprites[1]->SetIsFlipX(true);
	sprites[2]->SetIsFlipY(true);
	sprites[3]->SetIsFlipX(true);
	sprites[3]->SetIsFlipY(true);


	MSG msg{};
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		//Windowsメッセージ処理
		if (winApp->ProcessMessage()) {
			//ゲームループをぬける
			break;
		}
		//ゲームの処理
		input->Update();
		
#ifdef _DEBUG
		imGuiManager->Begin();

		
		//開発用のUIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		//ImGui::Begin("Settings");
		//ImGui::ShowDemoWindow();

		// ウインドウのサイズを固定する
		ImGui::SetNextWindowSize(ImVec2(500, 100));
		// ウインドウの位置を設定する
		ImGui::SetNextWindowPos(ImVec2(300, 100));
		// ウインドウフラグに NoResize を指定
		ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoResize);

		//if (ImGui::CollapsingHeader("Camera"))
		//{
		//	static ImGuiComboFlags flags = 0;
		//	const char* items[] = { "default","Camera2" };
		//	static int item_selected_idx = 0; // Here we store our selection data as an index.

		//	// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
		//	const char* combo_preview_value = items[item_selected_idx];

		//	if (ImGui::BeginCombo("Now Camera", combo_preview_value, flags))
		//	{
		//		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		//		{
		//			const bool is_selected = (item_selected_idx == n);
		//			if (ImGui::Selectable(items[n], is_selected)) {
		//				item_selected_idx = n;
		//				CameraManager::GetInstance()->FindCamera(items[n]);
		//			}

		//			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
		//			if (is_selected) {
		//				ImGui::SetItemDefaultFocus();
		//			}
		//		}
		//		ImGui::EndCombo();
		//	}

		//	Vector3 cameraRotate = CameraManager::GetInstance()->GetCamera()->GetRotate();
		//	ImGui::DragFloat3("Camera.Rotate", &cameraRotate.x, 0.001f);
		//	CameraManager::GetInstance()->GetCamera()->SetRotate(cameraRotate);

		//	Vector3 cameraPosition = CameraManager::GetInstance()->GetCamera()->GetTranslate();
		//	ImGui::DragFloat3("Camera.Translate", &cameraPosition.x, 0.01f);
		//	CameraManager::GetInstance()->GetCamera()->SetTranslate(cameraPosition);

		//	ImGui::Text("\n");
		//}
		//if (ImGui::CollapsingHeader("Object3d"))
		//{
		//	size_t object3dCount = object3ds.size() - 1;
		//	for (Object3d* object3d : object3ds) {
		//		Vector3 translate = object3d->GetTranslate();
		//		ImGui::DragFloat3("**Transform.Translate" + (char)object3dCount, &translate.x, 0.1f);
		//		object3d->SetTranslate(translate);

		//		Vector3 rotate = object3d->GetRotate();
		//		ImGui::SliderAngle("**Transform.Rotate.x" + (char)object3dCount, &rotate.x);
		//		ImGui::SliderAngle("**Transform.Rotate.y" + (char)object3dCount, &rotate.y);
		//		ImGui::SliderAngle("**Transform.Rotate.z" + (char)object3dCount, &rotate.z);
		//		object3d->SetRotate(rotate);

		//		Vector3 scale = object3d->GetScale();
		//		ImGui::DragFloat3("**Transform.Scale" + (char)object3dCount, &scale.x, 0.1f);
		//		object3d->SetScale(scale);

		//		//ImGui::ColorEdit4("Model.Color", &(*materialData).color.x);

		//		ImGui::Text("\n");

		//		object3dCount--;
		//	}
		//}
		//if (ImGui::CollapsingHeader("Sprite")){
			size_t spriteCount = 0;
			for (Sprite* sprite : sprites) {
				Vector2 position = sprite->GetPosition();
				ImGui::DragFloat2(("Sprite" + std::to_string(spriteCount) + ".Translate").c_str(), &position.x, 1.0f, 0.0f, 1180.0f, "%.1f");
				/*if (position.y > 640.0f) {
					position.y = 640.0f;
				}*/
				sprite->SetPosition(position);

				float rotation = sprite->GetRotation();
				ImGui::SliderAngle(("Sprite" + std::to_string(spriteCount) + ".Rotate").c_str(), &rotation);
				sprite->SetRotation(rotation);

				Vector2 size = sprite->GetSize();
				ImGui::DragFloat2(("Sprite" + std::to_string(spriteCount) + ".Scale").c_str(), &size.x, 1.0f, 0.0f, 640.0f, "%.1f");
				if (size.y > 360.0f) {
					size.y = 360.0f;
				}
				sprite->SetSize(size);

				Vector4 color = sprite->GetColor();
				ImGui::ColorEdit4(("Sprite" + std::to_string(spriteCount) + ".Color").c_str(), &color.x);
				sprite->SetColor(color);

				ImGui::Text("\n");
				spriteCount++;
			}
		//}
		//ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		//ImGui::ColorEdit4("DirectionalLightData.Color", &directionalLightData->color.x);
		//ImGui::DragFloat3("DirectionalLightData.Direction", &directionalLightData->direction.x, 0.01f, -1.0f, 1.0f);
		//directionalLightData->direction = Vector3::Normalize(directionalLightData->direction);
		//ImGui::DragFloat("DirectionalLightData.Intensity", &directionalLightData->intensity, 0.01f, 0.0f, 1.0f);
		/*ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);*/
		ImGui::End();
		imGuiManager->End();
#endif //_DEBUG
		
		for (Object3d* object3d : object3ds) {
			object3d->Update();
		}
		for (Sprite* sprite : sprites) {
			sprite->Update();
		}
		//ImGuiの内部コマンドを生成する
		//ImGui::Render();

		//描画前処理
		dxCommon->PreDraw();

		srvManager->PreDraw();

		// コマンドリストの取得
		ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

		//Modelの描画準備Modelの描画に共通グラフィックコマンドを積む
		ModelManager::GetInstance()->DrawCommonSetting();

		CameraManager::GetInstance()->GetCamera()->Update();

		for (Object3d* object3d : object3ds) {
			object3d->Draw();
		}

		//Spriteの描画準備Spriteの描画に共通のグラフィックコマンドを積む
		spriteCommon->DrawCommonSetting();
		for (Sprite* sprite : sprites) {
			sprite->Draw();
		}

#ifdef _DEBUG
		//実際のcommandListのImGuiの描画コマンドを積む
		imGuiManager->Draw();
#endif //_DEBUG

		//描画後処理
		dxCommon->PostDraw();
	}

	//終了
	ModelManager::GetInstance()->Finalize();
	TextureManager::GetInstance()->Finalize();
	CameraManager::GetInstance()->Finalize();
	imGuiManager->Finalize();
	winApp->Finalize();
	
	//解放
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	delete spriteCommon;
	for (Object3d* object3d : object3ds) {
		delete object3d;
	}
	delete imGuiManager;
	delete srvManager;
	delete input;
	delete dxCommon;
	delete winApp;

	//CloseHandle(fenceEvent);

	return 0;
}