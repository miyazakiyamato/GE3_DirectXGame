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
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    for (std::pair<const std::string, std::unique_ptr<ParticleGroup>>& pair : particleGroups) {
        ParticleGroup& group = *pair.second;

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