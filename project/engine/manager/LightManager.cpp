#include "LightManager.h"
#include <cassert>
#include "DirectXCommon.h"
#include <numbers>

LightManager* LightManager::instance = nullptr;

LightManager* LightManager::GetInstance() {
	if (instance == nullptr) {
		instance = new LightManager;
	}
	return instance;
}

void LightManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	//DirectionalLightのリソースを作る。
	directionalLightResource_ = dxCommon_->CreateBufferResource(sizeof(DirectionalLight));
	//デフォルト値を書き込んでおく
	directionalLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 1.0f;
	directionalLightData_->isBlinnPhong = 1;
	//PointLightのリソースを作る。
	pointLightResource_ = dxCommon_->CreateBufferResource(sizeof(PointLight));
	//デフォルト値を書き込んでおく
	pointLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,0.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 1.0f;
	pointLightData_->decay = 1.0f;
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
	commandList->SetGraphicsRootConstantBufferView(5, pointLightResource_.Get()->GetGPUVirtualAddress());
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

void LightManager::SetPointLight(const PointLight& pointLight){
	pointLightData_->color = pointLight.color;
	pointLightData_->position = pointLight.position;
	pointLightData_->intensity = pointLight.intensity;
	pointLightData_->radius = pointLight.radius;
	pointLightData_->decay = pointLight.decay;
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