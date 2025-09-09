#include "LightManager.h"
#include <cassert>
#include <numbers>
#include "DirectXCommon.h"
#include "SrvUavManager.h"
#include <imgui.h>

LightManager* LightManager::instance = nullptr;

LightManager* LightManager::GetInstance() {
	if (instance == nullptr) {
		instance = new LightManager;
	}
	return instance;
}

void LightManager::Initialize(DirectXCommon* dxCommon, SrvUavManager* srvUavManager) {
	dxCommon_ = dxCommon;
	srvUavManager_ = srvUavManager;

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

	srvIndexForPointLight = srvUavManager_->Allocate();
	srvUavManager_->CreateSRVforStructuredBuffer(srvIndexForPointLight, pointLightResource_.Get(), kMaxPointLight, sizeof(PointLight));
	
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

	srvIndexForSpotLight = srvUavManager_->Allocate();
	srvUavManager_->CreateSRVforStructuredBuffer(srvIndexForSpotLight, spotLightResource_.Get(), kMaxSpotLight, sizeof(SpotLight));
}

void LightManager::Draw(){
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	//Lighting
	//DirectionalLight
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource_.Get()->GetGPUVirtualAddress());
	//PointLight
	srvUavManager_->SetGraphicsRootDescriptorTable(5, srvIndexForPointLight);
	//SpotLight
	srvUavManager_->SetGraphicsRootDescriptorTable(6, srvIndexForSpotLight);
}

void LightManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void LightManager::ImGuiUpdate(){
	std::string groupName = "Light";
	if (ImGui::BeginMenu(groupName.c_str())) {
		ImGui::ColorEdit4("DirectionalLight.Color", &directionalLightData_->color.x);
		ImGui::DragFloat3("DirectionalLight.Direction", &directionalLightData_->direction.x, 0.01f);
		directionalLightData_->direction = directionalLightData_->direction.Normalize();
		ImGui::DragFloat("DirectionalLight.Intensity", &directionalLightData_->intensity, 0.01f);
		bool IsBlinnPhong = (bool)directionalLightData_->isBlinnPhong;
		ImGui::Checkbox("IsBlinnPhong", &IsBlinnPhong);
		directionalLightData_->isBlinnPhong = IsBlinnPhong;

		ImGui::SliderInt("PointLightCount", &directionalLightData_->pointLightCount, 0, 10);
		ImGui::SliderInt("SpotLightCount", &directionalLightData_->spotLightCount, 0, 10);

		ImGui::Text("\n");
		uint32_t idIndex = 0;
		for (uint32_t index = 0; index < LightManager::GetInstance()->GetPointLightCount(); index++) {
			std::string name = "PointLight" + std::to_string(index);
			if (ImGui::CollapsingHeader(name.c_str())) {
				ImGui::PushID(idIndex);
				ImGui::ColorEdit4("PointLight.Color", &pointLightData_[index].color.x);
				ImGui::DragFloat3("PointLight.Position", &pointLightData_[index].position.x, 0.01f);
				ImGui::DragFloat("PointLight.Intensity", &pointLightData_[index].intensity, 0.01f);
				ImGui::DragFloat("PointLight.Radius", &pointLightData_[index].radius, 0.01f);
				ImGui::DragFloat("PointLight.Decay", &pointLightData_[index].decay, 0.01f);
				ImGui::PopID();
			}
			++idIndex;
		}
		ImGui::Text("\n");
		idIndex = 0;
		for (uint32_t index = 0; index < LightManager::GetInstance()->GetSpotLightCount(); index++) {
			std::string name = "SpotLight" + std::to_string(index);
			if (ImGui::CollapsingHeader(name.c_str())) {
				ImGui::PushID(idIndex);
				ImGui::ColorEdit4("SpotLight.Color", &spotLightData_[index].color.x);
				ImGui::DragFloat3("SpotLight.Position", &spotLightData_[index].position.x, 0.01f);
				ImGui::DragFloat("SpotLight.Intensity", &spotLightData_[index].intensity, 0.01f);
				ImGui::DragFloat3("SpotLight.Direction", &spotLightData_[index].direction.x, 0.01f);
				spotLightData_[index].direction = spotLightData_[index].direction.Normalize();
				ImGui::DragFloat("SpotLight.Distance", &spotLightData_[index].distance, 0.01f);
				ImGui::DragFloat("SpotLight.Decay", &spotLightData_[index].decay, 0.01f);
				ImGui::DragFloat("SpotLight.CosAngle", &spotLightData_[index].cosAngle, 0.01f);
				ImGui::DragFloat("SpotLight.CosFalloff", &spotLightData_[index].cosFalloffStart, 0.01f);
				ImGui::PopID();
			}
			++idIndex;
		}
		ImGui::EndMenu();
	}
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