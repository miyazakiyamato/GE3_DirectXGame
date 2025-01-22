#include "ParticleManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include <numbers>
#include "PipelineManager.h"


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

	//ランダムエンジンの初期化
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

    // インデックスリソースの生成
    indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * kParticleIndexNum);

    // インデックスバッファビューの生成
    indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = sizeof(uint32_t) * kParticleIndexNum;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
    indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;
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
                group.kNumInstance--;
                continue;
            }
            Matrix4x4 worldMatrix = Matrix4x4::MakeScaleMatrix(particle.transform.scale) * billboardMatrix * Matrix4x4::MakeTranslateMatrix(particle.transform.translate);
            //Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(particle.transform.scale, particle.transform.rotate, particle.transform.translate);;
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
    // コマンドリストの取得
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // 全てのパーティクルグループについて処理
    for (auto& [name, group] : particleGroups) {
        //パイプラインを設定
        PipelineManager::GetInstance()->DrawSetting(PipelineState::kParticle, group->blendMode_);
        commandList->IASetVertexBuffers(0, 1, &group->vertexBufferView);// VBVを設定
        commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
        // インスタンシングデータのSRVのDescriptorTableを設定
        srvManager_->SetGraphicsRootDescriptorTable(0, group->srvIndexForInstancing);
        //SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
        commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetSrvHandleGPU(group->materialData.textureFilePath));
        // DrawCall (インスタンシング描画)
        commandList->DrawIndexedInstanced(6, group->kNumInstance, 0, 0, 0);
    }
}

void ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath) {
    assert(particleGroups.count(name) == 0 && "ParticleGroup with this name already exists.");

    // パーティクルグループの作成と初期化
    auto group = std::make_unique<ParticleGroup>();
    //.objの参照しているテクスチャファイル読み込み
    TextureManager::GetInstance()->LoadTexture(textureFilePath);
    group->materialData.textureFilePath = textureFilePath;

    group->kNumInstance = 0;

    // 頂点リソースの生成
    group->vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * kParticleVertexNum);

    // 頂点バッファビューの生成
    group->vertexBufferView.BufferLocation = group->vertexResource->GetGPUVirtualAddress();
    group->vertexBufferView.SizeInBytes = sizeof(VertexData) * kParticleVertexNum;
    group->vertexBufferView.StrideInBytes = sizeof(VertexData);
    // 頂点リソースに頂点データを書き込む
    group->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->vertexData));
    //テクスチャの頂点
    const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath);
    textureSize_.x = static_cast<float>(metadata.width);
    textureSize_.y = static_cast<float>(metadata.height);

    float tex_left = textureLeftTop_.x / metadata.width;
    float tex_right = (textureLeftTop_.x + textureSize_.x) / metadata.width;
    float tex_top = textureLeftTop_.y / metadata.height;
    float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metadata.height;

    // 頂点データを設定（四角形を構成）
    group->vertexData[0] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { tex_left ,tex_bottom }, { 1.0f,1.0f,1.0f,1.0f } };//左下
    group->vertexData[1] = { { -0.5f,  0.5f, 0.0f, 1.0f }, { tex_left ,tex_top    }, { 1.0f,1.0f,1.0f,1.0f } };//左上
    group->vertexData[2] = { {  0.5f, -0.5f, 0.0f, 1.0f }, { tex_right,tex_bottom }, { 1.0f,1.0f,1.0f,1.0f } };//右下
    group->vertexData[3] = { {  0.5f,  0.5f, 0.0f, 1.0f }, { tex_right,tex_top    }, { 1.0f,1.0f,1.0f,1.0f } };//右上
    
    // TextureManagerからGPUハンドルを取得
    group->materialData.srvIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);

    //WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    group->instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kMaxInstance);
    //データを書き込む
    //書き込むためのアドレスを取得
    group->instancingResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&group->instancingData));
    //単位行列を書き込んでおく
    for (uint32_t index = 0; index < kMaxInstance; index++) {
        group->instancingData[index].World = Matrix4x4::MakeIdentity4x4();
        group->instancingData[index].WVP = Matrix4x4::MakeIdentity4x4();
        group->instancingData[index].color = { 1.0f,1.0f,1.0f,1.0f };//色を書き込む
    }

    group->srvIndexForInstancing = srvManager_->ALLocate();
    srvManager_->CreateSRVforStructuredBuffer(group->srvIndexForInstancing, group->instancingResource.Get(), kMaxInstance, sizeof(ParticleForGPU));

    particleGroups[name] = std::move(group);
}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count) {
	assert(particleGroups.count(name) > 0 && "ParticleGroup with this name does not exist.");
    
	ParticleGroup& group = *particleGroups[name];
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
    uint32_t nowInstance = group.kNumInstance;
    group.kNumInstance += count;
    if (group.kNumInstance + count >= kMaxInstance) {
        group.kNumInstance = kMaxInstance;
    }
    for (uint32_t i = nowInstance; i < group.kNumInstance; ++i) {
        group.particles.push_back(CreateNewParticle(randomEngine,position));
	}
}

ParticleManager::Particle ParticleManager::CreateNewParticle(std::mt19937& randomEngine,const Vector3& position){
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
    Particle particle{};
    particle.transform.scale = { 1.0f,1.0f,1.0f };
    particle.transform.rotate = { 0.0f,0.0f,0.0f };
    Vector3 popPosition = position;
    particle.transform.translate = popPosition;
    particle.velocity = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
    particle.color = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine), 1.0f };
    particle.lifeTime = 2.0f;
    return particle;
}
