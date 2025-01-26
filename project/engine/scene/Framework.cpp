#include "Framework.h"
#include "SceneFactory.h"

void Framework::Initialize(){
	(void)CoInitializeEx(0, COINIT_MULTITHREADED);

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	//DirecXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//入力の初期化
	input_ = Input::GetInstance();
	input_->Initialize(winApp);
	
	//SRVの初期化
	srvManager = new SrvManager();
	srvManager->Initialize(dxCommon);
	
	//ImGuiの初期化
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize(winApp, dxCommon, srvManager);

	// グローバル変数の読み込み
	globalVariables_ = GlobalVariables::GetInstance();
	globalVariables_->LoadFiles();

	//パイプラインマネージャ
	pipelineManager = PipelineManager::GetInstance();
	pipelineManager->Initialize(dxCommon);

	//テクスチャマネージャの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize(dxCommon, srvManager);

	//パーティクルマネージャの初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Initialize(dxCommon, srvManager);

	//モデルマネージャの初期化
	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialize(dxCommon);
	
	//ライトマネージャの初期化
	lightManager_ = LightManager::GetInstance();
	lightManager_->Initialize(dxCommon,srvManager);

	//カメラマネージャの初期化
	cameraManager_ = CameraManager::GetInstance();
	cameraManager_->Initialize();
	cameraManager_->SetCamera("default");
	cameraManager_->FindCamera("default");
	cameraManager_->GetCamera()->SetRotate({ 0.3f,0.0f,0.0f });
	cameraManager_->GetCamera()->SetTranslate({ 0.0f,4.0f,-10.0f });
	
	//オーディオマネージャの初期化
	audioManager_ = AudioManager::GetInstance();
	audioManager_->Initialize();

	//シーンマネージャの初期化
	sceneFactory_ = new SceneFactory();
	sceneManager_ = SceneManager::GetInstance();
	sceneManager_->SetSceneFactory(sceneFactory_);
}

void Framework::Finalize(){
	//終了
	sceneManager_->Finalize();
	audioManager_->Finalize();
	cameraManager_->Finalize();
	lightManager_->Finalize();
	modelManager_->Finalize();
	particleManager_->Finalize();
	textureManager_->Finalize();
	pipelineManager->Finalize();
	imGuiManager->Finalize();
	input_->Finalize();
	winApp->Finalize();

	//解放
	delete sceneFactory_;
	delete imGuiManager;
	delete srvManager;
	delete dxCommon;
	delete winApp;
}

void Framework::Update(){
	//ゲームの処理
	Input::GetInstance()->Update();

	imGuiManager->Begin();

	sceneManager_->Update();

	imGuiManager->End();
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
