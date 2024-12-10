#include "Light.h"
#include "DirectXCommon.h"

void Light::Initialize(DirectXCommon* dxCommon) {
	{
		dxCommon_ = dxCommon;
		//DirectionalLightのリソースを作る。
		directionalLightResource = dxCommon_->CreateBufferResource(sizeof(DirectionalLight));
		//デフォルト値を書き込んでおく
		directionalLightResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
		directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
		directionalLightData->direction = { 0.0f,-1.0f,0.0f };
		directionalLightData->intensity = 1.0f;
	}
}

void Light::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList * commandList = dxCommon_->GetCommandList();
	//Lighting
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.Get()->GetGPUVirtualAddress());
}
