//#pragma warning(push)
//#pragma warning(disable:4023)
//#pragma warning(pop)
#define _USE_MATH_DEFINES
#include <dxgidebug.h>
#include <cmath>
#include <vector>

#include "externals/DirectXTex/DirectXTex.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

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
#include "ModelCommon.h"
#include "Model.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Microsoft::WRL;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	(void)CoInitializeEx(0, COINIT_MULTITHREADED);
	D3DResourceLeakChecker leakCheck;

	//ポインタ
	WinApp* winApp = nullptr;
	Input* input = nullptr;
	DirectXCommon* dxCommon = nullptr;
	ModelCommon* modelCommon = nullptr;
	Model* model = nullptr;
	SpriteCommon* spriteCommon = nullptr;
	std::vector<Sprite*> sprites;
	

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//DirecXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

	//モデル共通部の初期化
	modelCommon = new ModelCommon;
	modelCommon->Initialize(dxCommon);

	model = new Model;
	model->Initialize(modelCommon, "Resources", "axis.obj");

	//スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

	//スプライトの初期化
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite;
		sprite->Initialize(spriteCommon,"Resources/uvChecker.png");
		sprite->SetPosition({200.0f * float(i), 100});
		sprite->SetSize({ 100.0f,100.0f });
		sprites.push_back(sprite);
	}
	sprites[0]->SetTextureSize({64.0f,64.0f});
	sprites[1]->SetTexture("Resources/monsterBall.png");
	sprites[1]->SetIsFlipX(true);
	sprites[2]->SetIsFlipY(true);
	sprites[3]->SetIsFlipX(true);
	sprites[3]->SetIsFlipY(true);

	////Sphereの頂点数
	//const uint32_t kSubdivision = 16;
	//const uint32_t kVertexNum = kSubdivision * kSubdivision * 4;
	//ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device, sizeof(VertexData) * kVertexNum);
	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	////リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点3つのサイズ
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * kVertexNum;
	////1頂点当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	////頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	////書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	////経度分割1つ分の角度
	//const float KLonEvery = (float)M_PI * 2.0f / float(kSubdivision);
	////緯度分割1つ分の角度
	//const float KLatEvery = (float)M_PI / float(kSubdivision);
	////緯度の方向に分割
	//for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
	//	float lat = -(float)M_PI / 2.0f + KLatEvery * latIndex;//0
	//	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
	//		uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
	//		float lon = lonIndex * KLonEvery;//
	//		float u = float(lonIndex) / float(kSubdivision);
	//		float v = 1.0f - float(latIndex) / float(kSubdivision);
	//		//頂点にデータを入力する。
	//		//左下
	//		vertexData[start].position.x = cos(lat) * cos(lon);
	//		vertexData[start].position.y = sin(lat);
	//		vertexData[start].position.z = cos(lat) * sin(lon);
	//		vertexData[start].position.w = 1.0f;
	//		vertexData[start].texcoord = { u - 1.0f / float(kSubdivision), v};
	//		vertexData[start].normal.x = vertexData[start].position.x;
	//		vertexData[start].normal.y = vertexData[start].position.y;
	//		vertexData[start].normal.z = vertexData[start].position.z;
	//		//左上
	//		vertexData[start + 1].position.x = cos(lat + KLatEvery) * cos(lon);
	//		vertexData[start + 1].position.y = sin(lat + KLatEvery);
	//		vertexData[start + 1].position.z = cos(lat + KLatEvery) * sin(lon);
	//		vertexData[start + 1].position.w = 1.0f;
	//		vertexData[start + 1].texcoord = { u - 1.0f / float(kSubdivision), v - 1.0f / float(kSubdivision) };
	//		vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
	//		vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
	//		vertexData[start + 1].normal.z = vertexData[start + 1].position.z;
	//		//右下
	//		vertexData[start + 2].position.x = cos(lat) * cos(lon + KLonEvery);
	//		vertexData[start + 2].position.y = sin(lat);
	//		vertexData[start + 2].position.z = cos(lat) * sin(lon + KLonEvery);
	//		vertexData[start + 2].position.w = 1.0f;
	//		vertexData[start + 2].texcoord = { u, v};
	//		vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
	//		vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
	//		vertexData[start + 2].normal.z = vertexData[start + 2].position.z;
	//		//右上
	//		vertexData[start + 3].position.x = cos(lat + KLatEvery) * cos(lon + KLonEvery);
	//		vertexData[start + 3].position.y = sin(lat + KLatEvery);
	//		vertexData[start + 3].position.z = cos(lat + KLatEvery) * sin(lon + KLonEvery);
	//		vertexData[start + 3].position.w = 1.0f;
	//		vertexData[start + 3].texcoord = { u, v - 1.0f / float(kSubdivision) };
	//		vertexData[start + 3].normal.x = vertexData[start + 3].position.x;
	//		vertexData[start + 3].normal.y = vertexData[start + 3].position.y;
	//		vertexData[start + 3].normal.z = vertexData[start + 3].position.z;
	//	}
	//}

	////Sphere用のインデックスリソースを作る
	//const uint32_t kIndexNum = kSubdivision * kSubdivision * 6;
	//ComPtr<ID3D12Resource> indexResource = CreateBufferResource(device, sizeof(uint32_t) * kIndexNum);
	////IndexBufferView(IBV)の作成
	//D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	////リソースの先頭のアドレスから使う
	//indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズはインデックス6つ分のサイズ
	//indexBufferView.SizeInBytes = sizeof(uint32_t) * kIndexNum;
	////インデックスはuint32_tとする
	//indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	////インデックスリソースにデータを書き込む
	//uint32_t* indexData = nullptr;
	//indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	//for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
	//	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
	//		uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
	//		uint32_t vertex = (latIndex * kSubdivision + lonIndex) * 4;
	//		indexData[start] = vertex; indexData[start + 1] = vertex + 1; indexData[start + 2] = vertex + 2;
	//		indexData[start + 3] = vertex + 1; indexData[start + 4] = vertex + 3; indexData[start + 5] = vertex + 2;
	//	}
	//}

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

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//開発用のUIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		ImGui::Begin("Settings");
		//ImGui::ColorEdit4("Model.Color", &(*materialData).color.x);
		
		//ImGui::DragFloat3("Transform.Rotate", &transform.rotate.x, 0.01f, -2.0f * float(M_PI), 2.0f * float(M_PI));
		
		size_t spriteCount = sprites.size() - 1;
		for (Sprite* sprite : sprites) {
			Vector2 position = sprite->GetPosition();
			ImGui::DragFloat2("****Sprite.Translate" + (char)spriteCount, &position.x, 1.0f, 0.0f, 640.0f);
			if (position.y > 360.0f) {
				position.y = 360.0f;
			}
			sprite->SetPosition(position);

			float rotation = sprite->GetRotation();
			ImGui::SliderAngle("****Sprite.Rotate" + (char)spriteCount, &rotation);
			sprite->SetRotation(rotation);

			Vector2 size = sprite->GetSize();
			ImGui::DragFloat2("****Sprite.Scale" + (char)spriteCount, &size.x, 1.0f, 0.0f, 640.0f);
			if (size.y > 360.0f) {
				size.y = 360.0f;
			}
			sprite->SetSize(size);

			Vector4 color = sprite->GetColor();
			ImGui::ColorEdit4("****Sprite.Color" + (char)spriteCount, &color.x);
			sprite->SetColor(color);

			ImGui::Text("\n");
			spriteCount--;
		}
		/*ImGui::DragFloat3("Camera.Rotate", &cameraTransform.rotate.x, 0.001f);
		ImGui::DragFloat3("Camera.Translate", &cameraTransform.translate.x, 0.01f);*/
		//ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		//ImGui::ColorEdit4("DirectionalLightData.Color", &directionalLightData->color.x);
		//ImGui::DragFloat3("DirectionalLightData.Direction", &directionalLightData->direction.x, 0.01f, -1.0f, 1.0f);
		//directionalLightData->direction = Vector3::Normalize(directionalLightData->direction);
		//ImGui::DragFloat("DirectionalLightData.Intensity", &directionalLightData->intensity, 0.01f, 0.0f, 1.0f);
		/*ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);*/
		ImGui::End();

		//if (input->PushKey(DIK_A)) {
		//	cameraTransform.translate.x -= 0.1f;
		//}
		//else if (input->PushKey(DIK_D)) {
		//	cameraTransform.translate.x += 0.1f;
		//}
		//if (input->PushKey(DIK_W)) {
		//	cameraTransform.translate.y += 0.1f;
		//}
		//else if (input->PushKey(DIK_S)) {
		//	cameraTransform.translate.y -= 0.1f;
		//}
		model->Update();
		
		for (Sprite* sprite : sprites) {
			sprite->Update();
		}
		//ImGuiの内部コマンドを生成する
		ImGui::Render();

		//描画前処理
		dxCommon->PreDraw();
		// コマンドリストの取得
		ComPtr<ID3D12GraphicsCommandList> commandList = dxCommon->GetCommandList();

		//Modelの描画準備Modelの描画に共通グラフィックコマンドを積む
		modelCommon->DrawCommonSetting();

		model->Draw();
		
		//Spriteの描画準備Spriteの描画に共通のグラフィックコマンドを積む
		spriteCommon->DrawCommonSetting();
		for (Sprite* sprite : sprites) {
			sprite->Draw();
		}
		//実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

		//描画後処理
		dxCommon->PostDraw();
	}

	//ImGuiの終了処理。
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
	//終了
	winApp->Finalize();
	TextureManager::GetInstance()->Finalize();

	//解放
	
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	delete spriteCommon;
	delete model;
	delete modelCommon;
	delete input;
	delete dxCommon;
	delete winApp;

	//CloseHandle(fenceEvent);

	return 0;
}