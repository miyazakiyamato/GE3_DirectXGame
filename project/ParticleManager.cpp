#include "ParticleManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "CameraManager.h"


ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new ParticleManager;
	}
	return instance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager) {
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	particleCommon_ = std::make_unique<ParticleCommon>();
	particleCommon_->Initialize(dxCommon_);

	//ランダムエンジンの初期化
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	//頂点データの初期化
	//頂点リソースの生成
	//頂点バッファビューの生成
	//頂点リソースに頂点データを書き込む

	for (auto& [name, group] : particleGroups) {
		//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
		group->instancingResource = dxCommon->CreateBufferResource(sizeof(ParticleForGPU) * group->kNumInstance);
		//書き込むためのアドレスを取得
		group->instancingResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&group->instancingData));
		
		for (uint32_t index = 0; index < group->kNumInstance; ++index) {
			group->instancingData[index].WVP = Matrix4x4::MakeIdentity4x4();
			group->instancingData[index].World = Matrix4x4::MakeIdentity4x4();
			group->instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}

void ParticleManager::Finalize() {
	delete instance;
	instance = nullptr;
}

//void ParticleManager::Update(){
//	//ビルボード行列の計算
//
//	//ビュー行列とプロジェクション行列をカメラから取得
//
//	//全てのパーティクルグループについて処理する
//	//グループ内の全てのパーティクルについて処理する
//	
//	//寿命に達していたらグループから外す
//
//	//場の影響を計算(加速)
//
//	//移動処理(速度を座標に加算)
//
//	//ワールド行列を計算
//
//	//ワールドビュープロジェクション行列を合成
//
//	//インスタんシング用データ1個分の書き込み
//
//	//全パーティーグループ内の全パーティクルについて2重for文で処理する
//}
//
//void ParticleManager::Draw(){
//	// コマンドリストの取得
//	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
//
//	//ルートシグネチャを設定
//
//	//PSOを設定
//
//	//プリミティブトポロジーを設定
//
//	//VBVを設定
//
//	//全てのパーティクルグループについて処理する
//	//テクスチャのSRVのDesctiptorTableを設定
//	
//	//インスタんシングデータのSRVのDescriptorTableを設定
//
//	//DrawCall(インスタんシング描画)
//
//	//1グループで1DrawCallなので、こちらは二重for文にはならない
//}
//
//void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath){
//	//登録済みの名前かチェックしてassert
//
//	//新たな空のパーティクルグループを作成してコンテナに登録
//
//	//新たなパーティクルグループの初期化
//	//マテリアルデータにテクスチャファイルパスを設定
//
//	//マテリアルデータのテクスチャのSRVインデックスを記録
//
//	//インスタンシング用リソースの生成
//
//	//インスタンシング用にSRVを確保してSRVインデックスを記録
//
//	//SRV生成(StructuredBuffer用設定)
//}
//
//void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count){
//	//登録済みのパーティクルグループ名かチェックしてassert
//
//	//新たなパーティクルを作成して指定されたパーティクルグループに登録
//}

void ParticleManager::Update() {
	Matrix4x4 worldMatrix;
	Matrix4x4 worldViewProjectionMatrix;
	Matrix4x4 viewProjectionMatrix;
	Matrix4x4 billboardMatrix;

    for (auto& [name, group] : particleGroups) {

		for (uint32_t index = 0; index < group->kNumInstance; ++index) {
			// 寿命チェック
			if (group->particles[index].GetIsAlive() == false) {
				group->particles[index] = particles.erase(group->particles[index]);
				continue;
			}

			// パーティクルの位置や速度の更新
			group->particles[index].Update();

			worldMatrix = Matrix4x4::MakeAffineMatrix(group->particles[index].GetScale(), group->particles[index].GetRotate, group->particles[index].GetTranslate);
			if (CameraManager::GetInstance()->GetCamera()) {
				viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
				billboardMatrix = CameraManager::GetInstance()->GetCamera()->GetWorldMatrix();
				billboardMatrix.m[3][0] = 0.0f;
				billboardMatrix.m[3][1] = 0.0f;
				billboardMatrix.m[3][2] = 0.0f;
				worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
			}
			else {
				worldViewProjectionMatrix = worldMatrix;
			}
			group->instancingData[index].World = worldMatrix;
			group->instancingData[index].WVP = worldViewProjectionMatrix;
			group->instancingData[index].color = group->particles[index].GetColor();
		}
    }
}

void ParticleManager::Draw() {
	particleCommon_->DrawCommonSetting();

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    for (auto& [name, group] : particleGroups) {
       // ルートシグネチャにテクスチャのSRVを設定
        commandList->SetGraphicsRootDescriptorTable(0, group->materialData.srvHandleGPU);

        // インスタンシング用SRVを設定
        srvManager_->SetGraphicsRootDescriptorTable(1, group->srvIndexForInstancing);

        // インスタンシング描画のコマンドを発行
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(group->kNumInstance, 1, 0, 0);
    }
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath) {
    assert(particleGroups.count(name) == 0 && "ParticleGroup with this name already exists.");

    // パーティクルグループの作成と初期化
    auto group = std::make_unique<ParticleGroup>();
    group->materialData.textureFilePath = textureFilePath;

    // TextureManagerからGPUハンドルを取得
    group->materialData.srvHandleGPU = TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath);

    // インスタンシングバッファの作成
    group->instancingResource = dxCommon_->CreateBufferResource(group->kNumInstance * sizeof(ParticleForGPU));
    group->srvIndexForInstancing = srvManager_->ALLocate();
    srvManager_->CreateSRVforStructuredBuffer(group->srvIndexForInstancing, group->instancingResource.Get(), group->kNumInstance, sizeof(ParticleForGPU));

    particleGroups[name] = std::move(group);
}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count) {
	assert(particleGroups.count(name) > 0 && "ParticleGroup with this name does not exist.");

	ParticleGroup& group = *particleGroups[name];
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

	for (uint32_t i = 0; i < count; ++i) {
		Particle particle;
		particle.Initialize(group.materialData.textureFilePath);
		particle.SetScale({ 1.0f,1.0f,1.0f });
		particle.SetRotation({ 0.0f,0.0f,0.0f });
		particle.SetPosition(position);
		particle.SetVelocity({ distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) });
		particle.SetLifeTime(100.0f);
		group.particles.push_back(particle);
	}
}