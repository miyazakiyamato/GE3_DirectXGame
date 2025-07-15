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

}

void ParticleManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ParticleManager::Update() {

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
			particle.transform.scale += particle.velocity.scale * TimeManager::GetInstance()->deltaTime_;
			particle.transform.rotate += particle.velocity.rotate * TimeManager::GetInstance()->deltaTime_;
			particle.transform.translate += particle.velocity.translate * TimeManager::GetInstance()->deltaTime_;
			// UV変形の更新
			particle.uvTransform.scale += particle.uvVelocity.scale * TimeManager::GetInstance()->deltaTime_;
			particle.uvTransform.rotate += particle.uvVelocity.rotate * TimeManager::GetInstance()->deltaTime_;
			particle.uvTransform.translate += particle.uvVelocity.translate * TimeManager::GetInstance()->deltaTime_;
			particle.color.w = 1.0f - (particle.currentTime / particle.lifeTime);
			// パーティクルの時間更新
			particle.currentTime += TimeManager::GetInstance()->deltaTime_;
			if (particle.lifeTime < particle.currentTime) {
				it = group.particles.erase(it);
				group.kNumInstance--;
				continue;
			}
			if (particle.isBillboard) {
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
		if (group->kNumInstance == 0) {
			continue; // インスタンスがない場合は描画しない
		}

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

void ParticleManager::Emit(const std::string name, const Vector3& position, ParticleInitData particleInitData){
	if (particleGroups.count(name) == 0) {
		return;
	}
	ParticleGroup& group = *particleGroups[name];

	uint32_t nowInstance = group.kNumInstance;
	group.kNumInstance += particleInitData.count;
	if (group.kNumInstance >= kMaxInstance) {
		group.kNumInstance = kMaxInstance;
	}
	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(particleInitData.textureFilePath);
	group.materialData.textureFilePath = particleInitData.textureFilePath;
	// TextureManagerからGPUハンドルを取得
	group.materialData.srvIndex = TextureManager::GetInstance()->GetSrvIndex(group.materialData.textureFilePath);
	for (uint32_t i = nowInstance; i < group.kNumInstance; ++i) {
		Particle particle{};
		// Transformの各要素をランダム化
		particle.transform.scale = Vector3::Random(randomEngine_, particleInitData.randomTransformMin.scale, particleInitData.randomTransformMax.scale);
		particle.transform.rotate = Vector3::Random(randomEngine_, particleInitData.randomTransformMin.rotate, particleInitData.randomTransformMax.rotate);
		particle.transform.translate = Vector3(position) + Vector3::Random(randomEngine_, particleInitData.randomTransformMin.translate, particleInitData.randomTransformMax.translate);
		particle.uvTransform.scale = { 1.0f,1.0f,1.0f };
		particle.uvTransform.rotate = { 0.0f,0.0f,0.0f };
		particle.uvTransform.translate = { 0.0f,0.0f,0.0f };
		particle.color = Vector4::Random(randomEngine_, particleInitData.randomColorMin, particleInitData.randomColorMax);
		particle.velocity.scale = Vector3::Random(randomEngine_, particleInitData.randomVelocityMin.scale, particleInitData.randomVelocityMax.scale);
		particle.velocity.rotate = Vector3::Random(randomEngine_, particleInitData.randomVelocityMin.rotate, particleInitData.randomVelocityMax.rotate);
		particle.velocity.translate = Vector3::Random(randomEngine_, particleInitData.randomVelocityMin.translate, particleInitData.randomVelocityMax.translate);
		particle.uvVelocity.scale = Vector3::Random(randomEngine_, particleInitData.randomUvVelocityMin.scale, particleInitData.randomUvVelocityMax.scale);
		particle.uvVelocity.rotate = Vector3::Random(randomEngine_, particleInitData.randomUvVelocityMin.rotate, particleInitData.randomUvVelocityMax.rotate);
		particle.uvVelocity.translate = Vector3::Random(randomEngine_, particleInitData.randomUvVelocityMin.translate, particleInitData.randomUvVelocityMax.translate);
		particle.isBillboard = particleInitData.isBillboard;
		particle.lifeTime = particleInitData.lifeTime;
		group.particles.push_back(particle);
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
