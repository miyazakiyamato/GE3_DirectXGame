#include "ParticleManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include <numbers>
#include "PipelineManager.h"
#include "TimeManager.h"

ParticleManager* ParticleManager::instance = nullptr;

ParticleManager* ParticleManager::GetInstance(){
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
	randomEngine_ = randomEngine;
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

			particle.currentTime += TimeManager::GetInstance()->deltaTime_;
			particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
			particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
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
		commandList->IASetIndexBuffer(&group->indexBufferView);//IBVを設定
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

	group->kParticleVertexNum = 4;
	group->kParticleIndexNum = 6;
	// 頂点リソースの生成
	group->vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * group->kParticleVertexNum);

	// 頂点バッファビューの生成
	group->vertexBufferView.BufferLocation = group->vertexResource->GetGPUVirtualAddress();
	group->vertexBufferView.SizeInBytes = sizeof(VertexData) * group->kParticleVertexNum;
	group->vertexBufferView.StrideInBytes = sizeof(VertexData);
	// 頂点リソースに頂点データを書き込む
	group->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->vertexData));
	//テクスチャの頂点
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath);
	group->textureSize_.x = static_cast<float>(metadata.width);
	group->textureSize_.y = static_cast<float>(metadata.height);

	float tex_left = group->textureLeftTop_.x / metadata.width;
	float tex_right = (group->textureLeftTop_.x + group->textureSize_.x) / metadata.width;
	float tex_top = group->textureLeftTop_.y / metadata.height;
	float tex_bottom = (group->textureLeftTop_.y + group->textureSize_.y) / metadata.height;

	// 頂点データを設定（四角形を構成）
	group->vertexData[0] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { tex_left ,tex_bottom }, { 1.0f,1.0f,1.0f,1.0f } };//左下
	group->vertexData[1] = { { -0.5f,  0.5f, 0.0f, 1.0f }, { tex_left ,tex_top    }, { 1.0f,1.0f,1.0f,1.0f } };//左上
	group->vertexData[2] = { {  0.5f, -0.5f, 0.0f, 1.0f }, { tex_right,tex_bottom }, { 1.0f,1.0f,1.0f,1.0f } };//右下
	group->vertexData[3] = { {  0.5f,  0.5f, 0.0f, 1.0f }, { tex_right,tex_top    }, { 1.0f,1.0f,1.0f,1.0f } };//右上
	group->vertexResource->Unmap(0, nullptr);

	// インデックスリソースの生成
	group->indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * group->kParticleIndexNum);

	// インデックスバッファビューの生成
	group->indexBufferView.BufferLocation = group->indexResource->GetGPUVirtualAddress();
	group->indexBufferView.SizeInBytes = sizeof(uint32_t) * group->kParticleIndexNum;
	group->indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	group->indexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->indexData));
	group->indexData[0] = 0; group->indexData[1] = 1; group->indexData[2] = 2;
	group->indexData[3] = 1; group->indexData[4] = 3; group->indexData[5] = 2;
	group->indexResource->Unmap(0, nullptr);

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
	
	uint32_t nowInstance = group.kNumInstance;
	group.kNumInstance += count;
	if (group.kNumInstance + count >= kMaxInstance) {
		group.kNumInstance = kMaxInstance;
	}
	for (uint32_t i = nowInstance; i < group.kNumInstance; ++i) {
		Particle particle{};
		particle.transform.scale = Vector3::Random(randomEngine_, group.particleInitData.randomScaleMin, group.particleInitData.randomScaleMax);
		particle.transform.rotate = Vector3::Random(randomEngine_, group.particleInitData.randomRotateMin, group.particleInitData.randomRotateMax);
		particle.transform.translate = position;
		particle.velocity = Vector3::Random(randomEngine_, group.particleInitData.randomVelocityMin, group.particleInitData.randomVelocityMax);
		particle.color = Vector4::Random(randomEngine_, group.particleInitData.randomColorMin, group.particleInitData.randomColorMax);
		particle.lifeTime = group.particleInitData.lifeTime;
		group.particles.push_back(particle);
	}
}
