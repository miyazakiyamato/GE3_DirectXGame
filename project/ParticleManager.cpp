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

    // 頂点データの初期化
    // 頂点リソースの生成
    vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * kParticleVertexNum);

    // 頂点バッファビューの生成
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(VertexData) * kParticleVertexNum;
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    // インデックスリソースの生成
    indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * kParticleIndexNum);

    // インデックスバッファビューの生成
    indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = sizeof(uint32_t) * kParticleIndexNum;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    // 頂点リソースに頂点データを書き込む
    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    // 頂点データを設定（四角形を構成）
    vertexData[0] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } };
    vertexData[1] = { { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };
    vertexData[2] = { {  0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } };
    vertexData[3] = { {  0.5f,  0.5f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };
    vertexResource->Unmap(0, nullptr);

    indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
    indexData[3] = 2; indexData[4] = 1; indexData[5] = 3;
    indexResource->Unmap(0, nullptr);
}

void ParticleManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ParticleManager::Update() {
    Matrix4x4 viewProjectionMatrix;
    Matrix4x4 billboardMatrix;

    if (CameraManager::GetInstance()->GetCamera()) {
        viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
        billboardMatrix = CameraManager::GetInstance()->GetCamera()->GetWorldMatrix();
        billboardMatrix.m[3][0] = 0.0f;
        billboardMatrix.m[3][1] = 0.0f;
        billboardMatrix.m[3][2] = 0.0f;
    }

    for (std::pair<const std::string, std::unique_ptr<ParticleGroup>>& pair : particleGroups) {
        ParticleGroup& group = *pair.second;
        int index = 0;

        for (std::list<Particle>::iterator it = group.particles.begin(); it != group.particles.end();) {
            Particle& particle = *it;

            particle.currentTime += kDeltaTime_;
            particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
            particle.transform.translate = particle.transform.translate + particle.velocity * kDeltaTime_;
            if (particle.lifeTime <= particle.currentTime) {
                it = group.particles.erase(it);
                continue;
                return;
            }

            Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(particle.transform.scale, particle.transform.rotate, particle.transform.translate);;
            Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
            group.instancingData[index].World = worldMatrix;
            group.instancingData[index].WVP = worldViewProjectionMatrix;
            group.instancingData[index].color = particle.color;

            ++it;
            ++index;
        }
    }
}

void ParticleManager::Draw() {
    //particleCommon_->DrawCommonSetting();
    //ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    //for (std::pair<const std::string, std::unique_ptr<ParticleGroup>>& pair : particleGroups) {
    //    ParticleGroup& group = *pair.second;

    //    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
    //    commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
    //    //マテリアルCBufferの場所を設定
    //    commandList->SetGraphicsRootConstantBufferView(0, group.instancingResource.Get()->GetGPUVirtualAddress());
    //    //TransformationMatrixCBufferの場所を設定
    //    commandList->SetGraphicsRootConstantBufferView(1, group.instancingResource.Get()->GetGPUVirtualAddress());

    //    srvManager_->SetGraphicsRootDescriptorTable(2, group.materialData.srvIndex);
    //    //commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //    commandList->DrawIndexedInstanced(6,group.kNumInstance, 0, 0, 0);
    //}
    // コマンドリストの取得
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // ルートシグネチャを設定
    particleCommon_->DrawCommonSetting();

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);// VBVを設定
    commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
    // プリミティブトポロジーを設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 全てのパーティクルグループについて処理
    for (auto& [name, group] : particleGroups) {
        // テクスチャのSRVのDescriptorTableを設定
        srvManager_->SetGraphicsRootDescriptorTable(0, group->materialData.srvIndex);

        // インスタンシングデータのSRVのDescriptorTableを設定
        srvManager_->SetGraphicsRootDescriptorTable(1, group->srvIndexForInstancing);

        // DrawCall (インスタンシング描画)
        commandList->DrawIndexedInstanced(6, group->kNumInstance, 0, 0, 0);
    }
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath) {
    assert(particleGroups.count(name) == 0 && "ParticleGroup with this name already exists.");

    // パーティクルグループの作成と初期化
    auto group = std::make_unique<ParticleGroup>();
    group->materialData.textureFilePath = textureFilePath;

    // TextureManagerからGPUハンドルを取得
    group->materialData.srvIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
    //インスタンス数
    group->kNumInstance = kMaxInstance;
    // インスタンシングバッファの作成
    group->instancingResource = dxCommon_->CreateBufferResource(group->kNumInstance * sizeof(ParticleForGPU));
    group->srvIndexForInstancing = srvManager_->ALLocate();
    srvManager_->CreateSRVforStructuredBuffer(group->srvIndexForInstancing, group->instancingResource.Get(), group->kNumInstance, sizeof(ParticleForGPU));
    
    //WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    group->instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * group->kNumInstance);
    //書き込むためのアドレスを取得
    group->instancingResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&group->instancingData));

    for (uint32_t index = 0; index < group->kNumInstance; ++index) {
        group->instancingData[index].WVP = Matrix4x4::MakeIdentity4x4();
        group->instancingData[index].World = Matrix4x4::MakeIdentity4x4();
        group->instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        group->instancingData[index].uvTransform = Matrix4x4::MakeIdentity4x4();
    }
    particleGroups[name] = std::move(group);
}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count) {
	assert(particleGroups.count(name) > 0 && "ParticleGroup with this name does not exist.");
    if (count >= kMaxInstance) {
        assert(0);
    }

	ParticleGroup& group = *particleGroups[name];
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);


	for (uint32_t i = 0; i < count; ++i) {
        Particle particle{};
		particle.transform.scale = { 1.0f,1.0f,1.0f };
		particle.transform.rotate = { 0.0f,0.0f,0.0f};
		particle.transform.translate = position;
		particle.velocity = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine)};
		particle.lifeTime = 100.0f;
		group.particles.push_back(particle);
	}
}