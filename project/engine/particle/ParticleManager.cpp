#include "ParticleManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include <numbers>
#include "PipelineManager.h"
#include "TimeManager.h"
#include "GlobalVariables.h"
#include <imgui.h>

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

	//調整項目の初期化
	InitializeGlobalVariables();
	ApplyGlobalVariables();
	/*for (auto& [name, group] : particleGroupCreateDates_) {
		if (group->particleType == "cylinder") {
			CreateCylinderParticleGroup(name, group->textureFilePath, 32, 1.0f, 1.0f, 3.0f);
		} else if (group->particleType == "ring") {
			CreateRingParticleGroup(name, group->textureFilePath, 32, 1.0f, 0.2f);
		} else {
			CreateParticleGroup(name, group->textureFilePath);
		}
	}*/
}

void ParticleManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ParticleManager::Update() {
	//調整項目の更新
	ApplyGlobalVariables();

	Matrix4x4 worldMatrix;
	Matrix4x4 viewProjectionMatrix;
	Matrix4x4 billboardMatrix;
	Matrix4x4 worldViewProjectionMatrix;
	
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

			/*particle.currentTime += TimeManager::GetInstance()->deltaTime_;
			particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
			particle.transform.translate = particle.transform.translate + particle.velocity * TimeManager::GetInstance()->deltaTime_;
			particle.uvTransform.translate.x += 0.001f;*/
			particle.currentTime += TimeManager::GetInstance()->deltaTime_;
			if (particle.lifeTime < particle.currentTime) {
				it = group.particles.erase(it);
				group.kNumInstance--;
				continue;
			}
			if (group.particleInitData.isBillboard) {
				worldMatrix = Matrix4x4::MakeScaleMatrix(particle.transform.scale) * billboardMatrix * Matrix4x4::MakeRotateZMatrix(particle.transform.rotate.z) * Matrix4x4::MakeTranslateMatrix(particle.transform.translate);
			} else {
				worldMatrix = Matrix4x4::MakeAffineMatrix(particle.transform.scale, particle.transform.rotate, particle.transform.translate);
			}
			worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;
			group.instancingData[index].World = worldMatrix;
			group.instancingData[index].WVP = worldViewProjectionMatrix;
			group.instancingData[index].color = particle.color;
			group.instancingData[index].uvTransform = Matrix4x4::MakeAffineMatrix(particle.uvTransform.scale, particle.uvTransform.rotate, particle.uvTransform.translate);;

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
		PipelineManager::GetInstance()->DrawSetting(group->pipelineStateName_);

		commandList->IASetVertexBuffers(0, 1, &group->vertexBufferView);// VBVを設定
		commandList->IASetIndexBuffer(&group->indexBufferView);//IBVを設定
		// インスタンシングデータのSRVのDescriptorTableを設定
		srvManager_->SetGraphicsRootDescriptorTable(0, group->srvIndexForInstancing);
		//SRVのDescriptorTableの先頭を設定、2はrootParameter[2]である
		commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetSrvHandleGPU(group->materialData.textureFilePath));
		// DrawCall (インスタンシング描画)
		commandList->DrawIndexedInstanced(group->kParticleIndexNum, group->kNumInstance, 0, 0, 0);
	}
}

void ParticleManager::CreateParticleGroup(const std::string name) {
	if (particleGroups.count(name) != 0) {
		return;
	}
	//assert(particleGroups.count(name) == 0 && "ParticleGroup with this name already exists.");
	// パーティクルグループの作成と初期化
	auto group = std::make_unique<ParticleGroup>();

	auto it = particleGroupCreateDates_.find(name);
	if (it != particleGroupCreateDates_.end() && it->second) {
		group->particleInitData = it->second->particleInitData;
		it->second->particleType = "plane";
		//.objの参照しているテクスチャファイル読み込み
		TextureManager::GetInstance()->LoadTexture(it->second->textureFilePath);
		group->materialData.textureFilePath = it->second->textureFilePath;
	}
	//group->particleInitData = particleGroupCreateDates_.find(name)->second->particleInitData;

	

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
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(group->materialData.textureFilePath);
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
	group->materialData.srvIndex = TextureManager::GetInstance()->GetSrvIndex(group->materialData.textureFilePath);

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


	PipelineState pipelineState;
	pipelineState.shaderName = "Particle";
	pipelineState.blendMode = group->blendMode_;
	pipelineState.cullMode = CullMode::kNone;//カリングなし
	pipelineState.depthMode = DepthMode::kReadOnly;//読み込み
	group->pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);

	particleGroups[name] = std::move(group);
}

void ParticleManager::CreateRingParticleGroup(const std::string name, const uint32_t& kDivide, const float& kOuterRadius, const float& kInnerRadius){
	if (particleGroups.count(name) != 0) {
		return;
	}

	// パーティクルグループの作成と初期化
	auto group = std::make_unique<ParticleGroup>();

	auto it = particleGroupCreateDates_.find(name);
	if (it != particleGroupCreateDates_.end() && it->second) {
		group->particleInitData = it->second->particleInitData;
		it->second->particleType = "ring";
		//.objの参照しているテクスチャファイル読み込み
		TextureManager::GetInstance()->LoadTexture(it->second->textureFilePath);
		group->materialData.textureFilePath = it->second->textureFilePath;
	}

	group->kNumInstance = 0;

	group->kParticleVertexNum = 4 * kDivide;
	group->kParticleIndexNum = 6 * kDivide;
	// 頂点リソースの生成
	group->vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * group->kParticleVertexNum);

	// 頂点バッファビューの生成
	group->vertexBufferView.BufferLocation = group->vertexResource->GetGPUVirtualAddress();
	group->vertexBufferView.SizeInBytes = sizeof(VertexData) * group->kParticleVertexNum;
	group->vertexBufferView.StrideInBytes = sizeof(VertexData);
	// 頂点リソースに頂点データを書き込む
	group->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->vertexData));
	//テクスチャの頂点
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(group->materialData.textureFilePath);
	group->textureSize_.x = static_cast<float>(metadata.width);
	group->textureSize_.y = static_cast<float>(metadata.height);

	/*float tex_left = group->textureLeftTop_.x / metadata.width;
	float tex_right = (group->textureLeftTop_.x + group->textureSize_.x) / metadata.width;
	float tex_top = group->textureLeftTop_.y / metadata.height;
	float tex_bottom = (group->textureLeftTop_.y + group->textureSize_.y) / metadata.height;*/

	//// 頂点データを設定（四角形を構成）
	//group->vertexData[0] = { { -0.5f, -0.5f, 0.0f, 1.0f }, { tex_left ,tex_bottom }, { 1.0f,1.0f,1.0f,1.0f } };//左下
	//group->vertexData[1] = { { -0.5f,  0.5f, 0.0f, 1.0f }, { tex_left ,tex_top    }, { 1.0f,1.0f,1.0f,1.0f } };//左上
	//group->vertexData[2] = { {  0.5f, -0.5f, 0.0f, 1.0f }, { tex_right,tex_bottom }, { 1.0f,1.0f,1.0f,1.0f } };//右下
	//group->vertexData[3] = { {  0.5f,  0.5f, 0.0f, 1.0f }, { tex_right,tex_top    }, { 1.0f,1.0f,1.0f,1.0f } };//右上

	// インデックスリソースの生成
	group->indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * group->kParticleIndexNum);

	// インデックスバッファビューの生成
	group->indexBufferView.BufferLocation = group->indexResource->GetGPUVirtualAddress();
	group->indexBufferView.SizeInBytes = sizeof(uint32_t) * group->kParticleIndexNum;
	group->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	group->indexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->indexData));
	
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kDivide);
	for (uint32_t index = 0; index < kDivide; index++) {
		float sin = std::sin(radianPerDivide * float(index));
		float cos = std::cos(radianPerDivide * float(index));
		float sinNext = std::sin(radianPerDivide * float(index + 1));
		float cosNext = std::cos(radianPerDivide * float(index + 1));
		float u = float(index) / float(kDivide);
		float uNext = float(index + 1) / float(kDivide);
		group->vertexData[index * 4 + 0] = { { kOuterRadius * -sin,     kOuterRadius * cos, 0.0f, 1.0f },     { u ,0.0f }, { 1.0f,1.0f,1.0f,1.0f } };//左下
		group->vertexData[index * 4 + 1] = { { kInnerRadius * -sin,     kInnerRadius * cos, 0.0f, 1.0f },     { u ,1.0f }, { 1.0f,1.0f,1.0f,1.0f } };//左上
		group->vertexData[index * 4 + 2] = { { kOuterRadius * -sinNext, kOuterRadius * cosNext, 0.0f, 1.0f }, { uNext ,0.0f }, { 1.0f,1.0f,1.0f,1.0f } };//右下
		group->vertexData[index * 4 + 3] = { { kInnerRadius * -sinNext, kInnerRadius * cosNext, 0.0f, 1.0f }, { uNext ,1.0f }, { 1.0f,1.0f,1.0f,1.0f } };//右上

		group->indexData[index * 6 + 0] = index * 4 + 0; group->indexData[index * 6 + 1] = index * 4 + 1; group->indexData[index * 6 + 2] = index * 4 + 2;
		group->indexData[index * 6 + 3] = index * 4 + 1; group->indexData[index * 6 + 4] = index * 4 + 3; group->indexData[index * 6 + 5] = index * 4 + 2;
	}
	group->vertexResource->Unmap(0, nullptr);
	group->indexResource->Unmap(0, nullptr);

	// TextureManagerからGPUハンドルを取得
	group->materialData.srvIndex = TextureManager::GetInstance()->GetSrvIndex(group->materialData.textureFilePath);

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

	PipelineState pipelineState;
	pipelineState.shaderName = "Particle";
	pipelineState.blendMode = group->blendMode_;
	pipelineState.cullMode = CullMode::kNone;//カリングなし
	pipelineState.depthMode = DepthMode::kReadOnly;//読み込み
	pipelineState.staticSamplersMode = StaticSamplersMode::kclamp;
	group->pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);

	particleGroups[name] = std::move(group);
}

void ParticleManager::CreateCylinderParticleGroup(const std::string name,const uint32_t& kDivide, const float& kTopRadius, const float& kBottomRadius, const float& kHeight){
	if (particleGroups.count(name) != 0) {
		return;
	}

	// パーティクルグループの作成と初期化
	auto group = std::make_unique<ParticleGroup>();

	auto it = particleGroupCreateDates_.find(name);
	if (it != particleGroupCreateDates_.end() && it->second) {
		group->particleInitData = it->second->particleInitData;
		it->second->particleType = "cylinder";
		//.objの参照しているテクスチャファイル読み込み
		TextureManager::GetInstance()->LoadTexture(it->second->textureFilePath);
		group->materialData.textureFilePath = it->second->textureFilePath;
	}
	//group->particleInitData = particleGroupCreateDates_.find(name)->second->particleInitData;

	group->kNumInstance = 0;

	group->kParticleVertexNum = 4 * kDivide;
	group->kParticleIndexNum = 6 * kDivide;
	// 頂点リソースの生成
	group->vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * group->kParticleVertexNum);

	// 頂点バッファビューの生成
	group->vertexBufferView.BufferLocation = group->vertexResource->GetGPUVirtualAddress();
	group->vertexBufferView.SizeInBytes = sizeof(VertexData) * group->kParticleVertexNum;
	group->vertexBufferView.StrideInBytes = sizeof(VertexData);
	// 頂点リソースに頂点データを書き込む
	group->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->vertexData));
	//テクスチャの頂点
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(group->materialData.textureFilePath);
	group->textureSize_.x = static_cast<float>(metadata.width);
	group->textureSize_.y = static_cast<float>(metadata.height);

	// インデックスリソースの生成
	group->indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * group->kParticleIndexNum);

	// インデックスバッファビューの生成
	group->indexBufferView.BufferLocation = group->indexResource->GetGPUVirtualAddress();
	group->indexBufferView.SizeInBytes = sizeof(uint32_t) * group->kParticleIndexNum;
	group->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	group->indexResource->Map(0, nullptr, reinterpret_cast<void**>(&group->indexData));

	
	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kDivide);
	const float flipY = 1.0f;
	for (uint32_t index = 0; index < kDivide; index++) {
		float sin = std::sin(radianPerDivide * float(index));
		float cos = std::cos(radianPerDivide * float(index));
		float sinNext = std::sin(radianPerDivide * float(index + 1));
		float cosNext = std::cos(radianPerDivide * float(index + 1));
		float u = float(index) / float(kDivide);
		float uNext = float(index + 1) / float(kDivide);
		group->vertexData[index * 4 + 1] = { { kTopRadius * -sin,   kHeight,kTopRadius * cos, 1.0f },     { u ,flipY - 0.0f }, { 1.0f, 1.0f, 1.0f,1.0f } };//左上 // normal{-sin,0.0f,cos}
		group->vertexData[index * 4 + 3] = { { kTopRadius * -sinNext, kHeight,kTopRadius * cosNext, 1.0f }, { uNext ,flipY - 0.0f }, { 1.0f,1.0f,1.0f,1.0f } };//右上 // {-sinNext,0.0f,cosNext}
		group->vertexData[index * 4 + 0] = { { kBottomRadius * -sin,  0.0f, kBottomRadius * cos, 1.0f },     { u ,flipY - 1.0f }, { 1.0f,1.0f,1.0f,1.0f } };//左下 // {-sinNext,0.0f,cosNext}
		group->vertexData[index * 4 + 2] = { { kBottomRadius * -sinNext, 0.0f,kBottomRadius * cosNext, 1.0f }, { uNext ,flipY - 1.0f }, { 1.0f, 1.0f,1.0f,1.0f } };//右下 // {-sinNext,0.0f,cosNext}

		group->indexData[index * 6 + 0] = index * 4 + 0; group->indexData[index * 6 + 1] = index * 4 + 1; group->indexData[index * 6 + 2] = index * 4 + 2;
		group->indexData[index * 6 + 3] = index * 4 + 1; group->indexData[index * 6 + 4] = index * 4 + 3; group->indexData[index * 6 + 5] = index * 4 + 2;

	}
	group->vertexResource->Unmap(0, nullptr);
	group->indexResource->Unmap(0, nullptr);

	// TextureManagerからGPUハンドルを取得
	group->materialData.srvIndex = TextureManager::GetInstance()->GetSrvIndex(group->materialData.textureFilePath);

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

	PipelineState pipelineState;
	pipelineState.shaderName = "Particle";
	pipelineState.blendMode = group->blendMode_;
	pipelineState.cullMode = CullMode::kNone;//カリングなし
	pipelineState.depthMode = DepthMode::kReadOnly;//読み込み
	pipelineState.staticSamplersMode = StaticSamplersMode::kclamp;
	group->pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);

	particleGroups[name] = std::move(group);
}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count) {
	if (particleGroups.count(name) == 0) {
		return;
	}
	//assert(particleGroups.count(name) > 0 && "ParticleGroup with this name does not exist.");
	
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
		particle.uvTransform.scale = {1.0f,1.0f,1.0f};
		particle.uvTransform.rotate = { 0.0f,0.0f,0.0f };
		particle.uvTransform.translate = {0.0f,0.0f,0.0f};
		particle.velocity = Vector3::Random(randomEngine_, group.particleInitData.randomVelocityMin, group.particleInitData.randomVelocityMax);
		particle.color = Vector4::Random(randomEngine_, group.particleInitData.randomColorMin, group.particleInitData.randomColorMax);
		particle.lifeTime = group.particleInitData.lifeTime;
		group.particles.push_back(particle);
	}
}
//調整項目の初期化
void ParticleManager::InitializeGlobalVariables(){
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Particle";
	// グループを追加
	globalVariables->CreateGroup(groupName);
	uint32_t objectIDIndex = 0;
	while (globalVariables->GetValue<std::string>(groupName, "\x01Name" + std::to_string(objectIDIndex)) != "") {
		std::string name = globalVariables->GetValue<std::string>(groupName, "\x01Name" + std::to_string(objectIDIndex));
		if (particleGroups.count(name) == 0) {
			particleGroupCreateDates_[name] = std::make_unique<ParticleGroupCreateData>();
		}
		++objectIDIndex;
	}
	objectIDIndex = 0;
	for (auto& [name, group] : particleGroupCreateDates_) {
		globalVariables->AddItem(groupName, "\x01Name" + std::to_string(objectIDIndex), name);
		globalVariables->AddItem(groupName, "\x02TextureFilePath" + std::to_string(objectIDIndex), group->textureFilePath);
		globalVariables->AddItem(groupName, "\x03ParticleType" + std::to_string(objectIDIndex), group->particleType);
		globalVariables->AddItem(groupName, "\x04RandomScaleMax" + std::to_string(objectIDIndex), group->particleInitData.randomScaleMax);
		globalVariables->AddItem(groupName, "\x05RandomScaleMin" + std::to_string(objectIDIndex), group->particleInitData.randomScaleMin);
		globalVariables->AddItem(groupName, "\x06RandomRotateMax" + std::to_string(objectIDIndex), group->particleInitData.randomRotateMax);
		globalVariables->AddItem(groupName, "\x07RandomRotateMin" + std::to_string(objectIDIndex), group->particleInitData.randomRotateMin);
		globalVariables->AddItem(groupName, "\x08RandomVelocityMax" + std::to_string(objectIDIndex), group->particleInitData.randomVelocityMax);
		globalVariables->AddItem(groupName, "\x09RandomVelocityMin" + std::to_string(objectIDIndex), group->particleInitData.randomVelocityMin);
		globalVariables->AddItem(groupName, "\x0ARandomColorMax" + std::to_string(objectIDIndex), group->particleInitData.randomColorMax);
		globalVariables->AddItem(groupName, "\x0BRandomColorMin" + std::to_string(objectIDIndex), group->particleInitData.randomColorMin);
		globalVariables->AddItem(groupName, "\x0CLifeTime" + std::to_string(objectIDIndex), group->particleInitData.lifeTime);
		globalVariables->AddItem(groupName, "\x0DIsBillboard" + std::to_string(objectIDIndex), group->particleInitData.isBillboard);
		++objectIDIndex;
	}
}
// 調整項目の適用
void ParticleManager::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	std::string groupName = "Particle";
	uint32_t objectIDIndex = 0;
	for (auto& [name, group] : particleGroupCreateDates_) {
		group->name = globalVariables->GetValue<std::string>(groupName, "\x01Name" + std::to_string(objectIDIndex));
		group->textureFilePath = globalVariables->GetValue<std::string>(groupName, "\x02TextureFilePath" + std::to_string(objectIDIndex));
		group->particleType = globalVariables->GetValue<std::string>(groupName, "\x03ParticleType" + std::to_string(objectIDIndex));
		group->particleInitData.randomScaleMax = globalVariables->GetValue<Vector3>(groupName, "\x04RandomScaleMax" + std::to_string(objectIDIndex));
		group->particleInitData.randomScaleMin = globalVariables->GetValue<Vector3>(groupName, "\x05RandomScaleMin" + std::to_string(objectIDIndex));
		group->particleInitData.randomRotateMax = globalVariables->GetValue<Vector3>(groupName, "\x06RandomRotateMax" + std::to_string(objectIDIndex));
		group->particleInitData.randomRotateMin = globalVariables->GetValue<Vector3>(groupName, "\x07RandomRotateMin" + std::to_string(objectIDIndex));
		group->particleInitData.randomVelocityMax = globalVariables->GetValue<Vector3>(groupName, "\x08RandomVelocityMax" + std::to_string(objectIDIndex));
		group->particleInitData.randomVelocityMin = globalVariables->GetValue<Vector3>(groupName, "\x09RandomVelocityMin" + std::to_string(objectIDIndex));
		group->particleInitData.randomColorMax = globalVariables->GetValue<Vector4>(groupName, "\x0ARandomColorMax" + std::to_string(objectIDIndex));
		group->particleInitData.randomColorMin = globalVariables->GetValue<Vector4>(groupName, "\x0BRandomColorMin" + std::to_string(objectIDIndex));
		group->particleInitData.lifeTime = globalVariables->GetValue<float>(groupName, "\x0CLifeTime" + std::to_string(objectIDIndex));
		group->particleInitData.isBillboard = globalVariables->GetValue<bool>(groupName, "\x0DIsBillboard" + std::to_string(objectIDIndex));
		++objectIDIndex;
	}
}

ParticleManager::ParticleGroup* ParticleManager::GetParticleGroup(std::string name) {
	if (particleGroups.count(name) == 0) {
		return nullptr;
	}
	return particleGroups[name].get();
}

void ParticleManager::SetBlendMode(std::string name, BlendMode blendMode) {
	particleGroups[name]->blendMode_ = blendMode;
	PipelineState pipelineState;
	pipelineState.shaderName = "Particle";
	pipelineState.blendMode = particleGroups[name]->blendMode_;
	pipelineState.cullMode = CullMode::kNone;//カリングなし
	pipelineState.depthMode = DepthMode::kReadOnly;//読み込み
	pipelineState.staticSamplersMode = StaticSamplersMode::kclamp;
	particleGroups[name]->pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);
}
void ParticleManager::UpdateGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Particle";
	//std::string blendName = "Now Blend";
	//std::vector<std::string> blendState{
	//	"None",      //!< ブレンドなし
	//	"Normal",    //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	//	"Add",       //!< 加算。Src * SrcA + Dest * 1
	//	"Subtract",  //!< 減算。Dest * 1 - Src * SrcA
	//	"Multiply",  //!< 乗算。Src * 0 + Dest * Src
	//	"Screen", };
	if (ImGui::BeginMenu(groupName)) {
		// テキスト入力ボックス
		if (ImGui::InputText("Input GroupNameText", buffer, IM_ARRAYSIZE(buffer))) {
			// 入力が変更された場合に反映
			groupNameText = buffer;
		}
		if (ImGui::InputText("Input TypeNameText", buffer2, IM_ARRAYSIZE(buffer2))) {
			// 入力が変更された場合に反映
			typeNameText = buffer2;
		}
		bool isGroupCreate = false;
		// 入力された文字列を表示
		ImGui::Text("Reflected Text: %s", groupNameText.c_str());
		if (groupNameText.empty()) {
			ImGui::Text("Error: Group name cannot be empty.");
		} else if (particleGroupCreateDates_.count(groupNameText) != 0) {
			ImGui::Text("Error: Group name already exists.");
		} else {
			isGroupCreate = true;
		}
		// ボタンを押したときの処理
		if (ImGui::Button("CreateGroup")) {
			if (isGroupCreate) {
				particleGroupCreateDates_[groupNameText] = std::make_unique<ParticleGroupCreateData>();
				InitializeGlobalVariables();
				if (typeNameText == "cylinder") {
					CreateCylinderParticleGroup(groupNameText, 32, 1.0f, 1.0f,3.0f);
				} else if (typeNameText == "ring") {
					CreateRingParticleGroup(groupNameText,32,1.0f,0.2f);
				} else {
					CreateParticleGroup(groupNameText);
				}
			}
		}
		//パーティクル初期化データの更新
		for (auto& [name, group] : particleGroups) {
			auto it = particleGroupCreateDates_.find(name);
			if (it != particleGroupCreateDates_.end() && it->second) {
				group->particleInitData = it->second->particleInitData;
			}
		}
		uint32_t objectIDIndex = 0;
		for (auto& [name, group] : particleGroupCreateDates_) {
			//テクスチャのキーを取得
			std::string textureName = name + ": Now Texture";
			std::vector<std::string> textureState = TextureManager::GetInstance()->GetKeys();
			auto it = particleGroups.find(name);
			if (it != particleGroups.end() && it->second) {
				//テクスチャ選択
				std::string textureItem_selected_idx = it->second->materialData.textureFilePath;
				const char* currentItem = textureItem_selected_idx.c_str();
				if (ImGui::BeginCombo((textureName + name).c_str(), currentItem)) {
					for (int i = 0; i < textureState.size(); ++i) {
						bool isSelected = (textureItem_selected_idx == textureState[i]);
						if (ImGui::Selectable(textureState[i].c_str(), isSelected)) {
							it->second->materialData.textureFilePath = textureState[i];

							group->textureFilePath = textureState[i];
							globalVariables->SetValue(groupName, "\x02TextureFilePath" + std::to_string(objectIDIndex), group->textureFilePath);
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				globalVariables->SetValue(groupName, "\x03ParticleType" + std::to_string(objectIDIndex), group->particleType);
			}
			//パーティクルの発生
			if (ImGui::Button(std::string(name + ": Emit").c_str())) {
				if (particleGroupCreateDates_.count(name) == 0) {
					ImGui::Text("Error: Group name is empty.");
				} else {
					Emit(name, Vector3(0.0f, 0.0f, 0.0f), 1);
				}
			}
			++objectIDIndex;
		}

	//	int particleItem_selected_idx = static_cast<int>(ParticleManager::GetInstance()->GetBlendMode(particleEmitter_->GetName()));
	//	const char* currentItem = blendState[particleItem_selected_idx].c_str();
	//	if (ImGui::BeginCombo((blendName + particleEmitter_->GetName()).c_str(), currentItem)) {
	//		for (int i = 0; i < blendState.size(); ++i) {
	//			bool isSelected = (particleItem_selected_idx == i);
	//			if (ImGui::Selectable(blendState[i].c_str(), isSelected)) {
	//				particleItem_selected_idx = i;
	//				ParticleManager::GetInstance()->SetBlendMode(particleEmitter_->GetName(), static_cast<BlendMode>(i));
	//			}
	//			if (isSelected) {
	//				ImGui::SetItemDefaultFocus();
	//			}
	//		}
	//		ImGui::EndCombo();
	//	}

	//	Vector3 position = particleEmitter_->GetPosition();
	//	ImGui::DragFloat2("particleEmitter_.Translate", &position.x, 0.1f);
	//	/*if (position.y > 640.0f) {
	//		position.y = 640.0f;
	//	}*/

	//	//Vector3 rotation = particleEmitter_->GetRotation();
	//	//ImGui::SliderAngle("particleEmitter_.Rotate", &rotation.x);

	//	//Vector3 size = particleEmitter_->GetSize();
	//	//ImGui::DragFloat2("particleEmitter_.Scale", &size.x, 0.1f);
	//	//if (size.y > 360.0f) {
	//	//	size.y = 360.0f;
	//	//}

	//	int count = particleEmitter_->GetCount();
	//	ImGui::DragInt("particleEmitter_.count", &count, 1, 0, 1000);

	//	float frequency = particleEmitter_->GetFrequency();
	//	ImGui::DragFloat("particleEmitter_.frequency", &frequency, 0.1f);

	//	if (ImGui::Button("ParticleEmit", { 100,50 })) {
	//		particleEmitter_->Emit();
	//	}

	//	bool isEmitUpdate = particleEmitter_->GetIsEmitUpdate();
	//	ImGui::Checkbox("IsEmitUpdate", &isEmitUpdate);

	//	particleEmitter_->SetPosition(position);
	//	//particleEmitter_->SetRotation(rotation);
	//	//particleEmitter_->SetSize(size);
	//	particleEmitter_->SetCount(count);
	//	particleEmitter_->SetFrequency(frequency);
	//	particleEmitter_->SetIsEmitUpdate(isEmitUpdate);

	//	//ImGui::Checkbox("IsAccelerationField", &isAccelerationField);
		ImGui::EndMenu();
	}
}
