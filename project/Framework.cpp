#include "Framework.h"

void Framework::Initialize(){
	(void)CoInitializeEx(0, COINIT_MULTITHREADED);

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	//DirecXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//入力の初期化
	input = new Input();
	input->Initialize(winApp);
	
	//SRVの初期化
	srvManager = new SrvManager();
	srvManager->Initialize(dxCommon);
	
	//ImGuiの初期化
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize(winApp, dxCommon, srvManager);

	//テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon, srvManager);

	//モデルマネージャの初期化
	ModelManager::GetInstance()->Initialize(dxCommon);
	
	//カメラマネージャの初期化
	CameraManager::GetInstance()->Initialize();
	CameraManager::GetInstance()->SetCamera("default");
	CameraManager::GetInstance()->FindCamera("default");
	CameraManager::GetInstance()->GetCamera()->SetRotate({ 0.3f,0.0f,0.0f });
	CameraManager::GetInstance()->GetCamera()->SetTranslate({ 0.0f,4.0f,-10.0f });

	//スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);
}

void Framework::Finalize(){
	//終了
	AudioManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	TextureManager::GetInstance()->Finalize();
	CameraManager::GetInstance()->Finalize();
	imGuiManager->Finalize();
	winApp->Finalize();

	//解放
	delete spriteCommon;
	delete imGuiManager;
	delete srvManager;
	delete input;
	delete dxCommon;
	delete winApp;

	//CloseHandle(fenceEvent);
}

void Framework::Update(){
	//ゲームの処理
	input->Update();
}

void Framework::Run(){
	//初期化
	Initialize();
	MSG msg{};
	//ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		//Windowsメッセージ処理
		if (winApp->ProcessMessage()) {
			//ゲームループをぬける
			break;
		}
		//毎フレーム更新
		Update();
		//描画
		Draw();
	}
	//終了処理
	Finalize();
}