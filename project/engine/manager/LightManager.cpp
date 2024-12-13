#include "LightManager.h"
#include <cassert>
#include "DirectXCommon.h"

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
}

void LightManager::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	//Lighting
	//DirectionalLight
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource_.Get()->GetGPUVirtualAddress());
}

void LightManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void LightManager::SetDirectionalLight(const DirectionalLight& directionalLight){
	 directionalLightData_->color = directionalLight.color;
	 directionalLightData_->direction = directionalLight.direction;
	 directionalLightData_->intensity = directionalLight.intensity;
}
