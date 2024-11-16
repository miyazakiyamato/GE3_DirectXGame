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

    //Sprite用の頂点リソースを作る
    vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * 6);

    //リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
    //使用するリソースのサイズは頂点6つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
    //1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    //IndexSprite用のインデックスリソースを作る
    indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * 6);

    //リソースの先頭のアドレスから使う
    indexBufferView.BufferLocation = indexResource.Get()->GetGPUVirtualAddress();
    //使用するリソースのサイズはインデックス6つ分のサイズ
    indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
    //インデックスはuint32_tとする
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
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
        //頂点リソースにデータを書き込む
        vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

        //三角形の頂点
        float left = 0.0f - anchorPoint_.x;
        float right = 1.0f - anchorPoint_.x;
        float top = 0.0f - anchorPoint_.y;
        float bottom = 1.0f - anchorPoint_.y;
        //頂点座標
        vertexData[0].position = { left,bottom,0.0f,1.0f };//左下
        vertexData[1].position = { left,top,0.0f,1.0f };//左上
        vertexData[2].position = { right,bottom,0.0f,1.0f };//右下
        vertexData[3].position = { right,top,0.0f,1.0f };//右上

        //テクスチャの頂点
        const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(group.materialData.textureFilePath);
        float tex_left = textureLeftTop_.x / metadata.width;
        float tex_right = (textureLeftTop_.x + textureSize_.x) / metadata.width;
        float tex_top = textureLeftTop_.y / metadata.height;
        float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metadata.height;

        //テクスチャ頂点座標
        vertexData[0].texcoord = { tex_left,tex_bottom };//左下
        vertexData[1].texcoord = { tex_left,tex_top };//左上
        vertexData[2].texcoord = { tex_right,tex_bottom };//右下
        vertexData[3].texcoord = { tex_right,tex_top };//右上

        //インデックスリソースにデータを書き込む
        indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
        indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
        indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

        for (std::list<Particle>::iterator it = group.particles.begin(); it != group.particles.end();) {
            Particle& particle = *it;

            particle.Update();

            if (!particle.GetIsAlive()) {
                it = group.particles.erase(it);
                continue;
            }

            Matrix4x4 worldMatrix = particle.GetWorldMatrix();
            Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
            group.instancingData[index].World = worldMatrix;
            group.instancingData[index].WVP = worldViewProjectionMatrix;
            group.instancingData[index].color = particle.GetColor();

            ++it;
            ++index;
        }
    }
}

void ParticleManager::Draw() {
    particleCommon_->DrawCommonSetting();
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    for (std::pair<const std::string, std::unique_ptr<ParticleGroup>>& pair : particleGroups) {
        ParticleGroup& group = *pair.second;

        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
        commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
        commandList->SetGraphicsRootDescriptorTable(0, group.materialData.srvHandleGPU);
        srvManager_->SetGraphicsRootDescriptorTable(1, group.srvIndexForInstancing);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawInstanced(group.kNumInstance, 1, 0, 0);
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
    
    //WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    group->instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * group->kNumInstance);
    //書き込むためのアドレスを取得
    group->instancingResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&group->instancingData));

    for (uint32_t index = 0; index < group->kNumInstance; ++index) {
        group->instancingData[index].WVP = Matrix4x4::MakeIdentity4x4();
        group->instancingData[index].World = Matrix4x4::MakeIdentity4x4();
        group->instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }
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