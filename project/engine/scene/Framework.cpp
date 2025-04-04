#include "Framework.h"
#include "SceneFactory.h"

void Framework::Initialize(){
	(void)CoInitializeEx(0, COINIT_MULTITHREADED);

	//WindowsAPIの初期化
	winApp.reset(new WinApp());
	winApp->Initialize();

	//DirecXの初期化
	dxCommon.reset(new DirectXCommon());
	dxCommon->Initialize(winApp.get());

	//入力の初期化
	input_ = Input::GetInstance();
	input_->Initialize(winApp.get());
	
	//SRVの初期化
	srvManager.reset(new SrvManager());
	srvManager->Initialize(dxCommon.get());
	
	//ImGuiの初期化
	imGuiManager.reset(new ImGuiManager());
	imGuiManager->Initialize(winApp.get(), dxCommon.get(), srvManager.get());

	// グローバル変数の読み込み
	globalVariables_ = GlobalVariables::GetInstance();
	globalVariables_->LoadFiles();

	//パイプラインマネージャ
	pipelineManager = PipelineManager::GetInstance();
	pipelineManager->Initialize(dxCommon.get());

	//テクスチャマネージャの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize(dxCommon.get(), srvManager.get());

	//パーティクルマネージャの初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Initialize(dxCommon.get(), srvManager.get());

	//モデルマネージャの初期化
	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialize(dxCommon.get(), srvManager.get());
	
	//ラインマネージャ
	lineManager_ = Line3dManager::GetInstance();
	lineManager_->Initialize(srvManager.get());

	//ライトマネージャの初期化
	lightManager_ = LightManager::GetInstance();
	lightManager_->Initialize(dxCommon.get(), srvManager.get());

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
	sceneFactory_.reset(new SceneFactory());
	sceneManager_ = SceneManager::GetInstance();
	sceneManager_->SetSceneFactory(sceneFactory_.get());

	//タイムマネージャの初期化
	timeManager_ = TimeManager::GetInstance();
	timeManager_->Initialize();

}

void Framework::Finalize(){
	//終了
	timeManager_->Finalize();
	sceneManager_->Finalize();
	audioManager_->Finalize();
	cameraManager_->Finalize();
	lightManager_->Finalize();
	lineManager_->Finalize();
	modelManager_->Finalize();
	particleManager_->Finalize();
	textureManager_->Finalize();
	pipelineManager->Finalize();
	imGuiManager->Finalize();
	input_->Finalize();
	winApp->Finalize();

	//解放
}

void Framework::Update(){
	//ゲームの処理
	Input::GetInstance()->Update();

	imGuiManager->Begin();


	lineManager_->Update();

	sceneManager_->Update();

	timeManager_->Update();

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
