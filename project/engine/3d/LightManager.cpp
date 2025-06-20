#include "LightManager.h"
#include <cassert>
#include <numbers>
#include "DirectXCommon.h"
#include "SrvManager.h"

LightManager* LightManager::instance = nullptr;

LightManager* LightManager::GetInstance() {
	if (instance == nullptr) {
		instance = new LightManager;
	}
	return instance;
}

void LightManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager) {
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	//DirectionalLightのリソースを作る。
	directionalLightResource_ = dxCommon_->CreateBufferResource(sizeof(DirectionalLight));
	//デフォルト値を書き込んでおく
	directionalLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 1.0f;
	directionalLightData_->isBlinnPhong = 1;
	
	//PointLightのリソースを作る。
	pointLightResource_ = dxCommon_->CreateBufferResource(sizeof(PointLight) * kMaxPointLight);
	//データを書き込む
	//書き込むためのアドレスを取得
	pointLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kMaxPointLight; index++) {
		pointLightData_[index].color = {1.0f,1.0f,1.0f,1.0f};
		pointLightData_[index].position = { 0.0f,0.0f,0.0f };
		pointLightData_[index].intensity = 1.0f;
		pointLightData_[index].radius = 1.0f;
		pointLightData_[index].decay = 1.0f;
	}

	srvIndexForPointLight = srvManager_->ALLocate();
	srvManager_->CreateSRVforStructuredBuffer(srvIndexForPointLight, pointLightResource_.Get(), kMaxPointLight, sizeof(PointLight));
	
	//SpotLightのリソースを作る。
	spotLightResource_ = dxCommon_->CreateBufferResource(sizeof(SpotLight) * kMaxSpotLight);
	//デフォルト値を書き込んでおく
	spotLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	for (uint32_t index = 0; index < kMaxSpotLight; index++) {
		spotLightData_[index].color = { 1.0f,1.0f,1.0f,1.0f };
		spotLightData_[index].position = { 2.0f,1.25f,0.0f };
		spotLightData_[index].intensity = 4.0f;
		spotLightData_[index].direction = Vector3(-1.0f, -1.0f, 0.0f).Normalize();
		spotLightData_[index].distance = 7.0f;
		spotLightData_[index].decay = 2.0f;
		spotLightData_[index].cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
		spotLightData_[index].cosFalloffStart = 1.0f;
	}

	srvIndexForSpotLight = srvManager_->ALLocate();
	srvManager_->CreateSRVforStructuredBuffer(srvIndexForSpotLight, spotLightResource_.Get(), kMaxSpotLight, sizeof(SpotLight));
}

void LightManager::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	//Lighting
	//DirectionalLight
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource_.Get()->GetGPUVirtualAddress());
	//PointLight
	srvManager_->SetGraphicsRootDescriptorTable(5, srvIndexForPointLight);
	//SpotLight
	srvManager_->SetGraphicsRootDescriptorTable(6, srvIndexForSpotLight);
}

void LightManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void LightManager::SetDirectionalLight(const DirectionalLight& directionalLight){
	 directionalLightData_->color = directionalLight.color;
	 directionalLightData_->direction = directionalLight.direction;
	 directionalLightData_->intensity = directionalLight.intensity;
	 directionalLightData_->isBlinnPhong = directionalLight.isBlinnPhong;
	 directionalLightData_->pointLightCount = directionalLight.pointLightCount < (int)kMaxPointLight ? directionalLight.pointLightCount : (int)kMaxPointLight;
	 directionalLightData_->spotLightCount = directionalLight.spotLightCount < (int)kMaxSpotLight ? directionalLight.spotLightCount : (int)kMaxSpotLight;
}

void LightManager::SetPointLight(uint32_t index, const PointLight& pointLight){
	pointLightData_[index].color = pointLight.color;
	pointLightData_[index].position = pointLight.position;
	pointLightData_[index].intensity = pointLight.intensity;
	pointLightData_[index].radius = pointLight.radius;
	pointLightData_[index].decay = pointLight.decay;
}

void LightManager::SetSpotLight(uint32_t index, const SpotLight& spotLight){
	spotLightData_[index].color = spotLight.color;
	spotLightData_[index].position = spotLight.position;
	spotLightData_[index].intensity = spotLight.intensity;
	spotLightData_[index].direction = spotLight.direction;
	spotLightData_[index].distance = spotLight.distance;
	spotLightData_[index].decay = spotLight.decay;
	spotLightData_[index].cosAngle = spotLight.cosAngle;
	spotLightData_[index].cosFalloffStart = spotLight.cosFalloffStart;
}