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
		pointLightData_[index].intensity = 0.0f;
		pointLightData_[index].radius = 1.0f;
		pointLightData_[index].decay = 1.0f;
	}

	srvIndexForPointLight = srvManager_->ALLocate();
	srvManager_->CreateSRVforStructuredBuffer(srvIndexForPointLight, pointLightResource_.Get(), kMaxPointLight, sizeof(PointLight));
	
	//SpotLightのリソースを作る。
	spotLightResource_ = dxCommon_->CreateBufferResource(sizeof(SpotLight));
	//デフォルト値を書き込んでおく
	spotLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_->position = { 2.0f,1.25f,0.0f };
	spotLightData_->intensity = 4.0f;
	spotLightData_->direction = Vector3( -1.0f,-1.0f,0.0f).Normalize();
	spotLightData_->distance = 7.0f;
	spotLightData_->decay = 2.0f;
	spotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData_->cosFalloffStart = 1.0f;
}

void LightManager::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	//Lighting
	//DirectionalLight
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource_.Get()->GetGPUVirtualAddress());
	//PointLight
	srvManager_->SetGraphicsRootDescriptorTable(5, srvIndexForPointLight);
	//commandList->SetGraphicsRootConstantBufferView(5, pointLightResource_.Get()->GetGPUVirtualAddress());
	//SpotLight
	commandList->SetGraphicsRootConstantBufferView(6, spotLightResource_.Get()->GetGPUVirtualAddress());
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
}

void LightManager::SetPointLight(uint32_t index, const PointLight& pointLight){
	pointLightData_[index].color = pointLight.color;
	pointLightData_[index].position = pointLight.position;
	pointLightData_[index].intensity = pointLight.intensity;
	pointLightData_[index].radius = pointLight.radius;
	pointLightData_[index].decay = pointLight.decay;
}

void LightManager::SetSpotLight(const SpotLight& spotLight){
	spotLightData_->color = spotLight.color;
	spotLightData_->position = spotLight.position;
	spotLightData_->intensity = spotLight.intensity;
	spotLightData_->direction = spotLight.direction;
	spotLightData_->distance = spotLight.distance;
	spotLightData_->decay = spotLight.decay;
	spotLightData_->cosAngle = spotLight.cosAngle;
	spotLightData_->cosFalloffStart = spotLight.cosFalloffStart;
}