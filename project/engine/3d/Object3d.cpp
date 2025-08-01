#include "Object3d.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "PipelineManager.h"
#include "TimeManager.h"
#include "Line3D.h"
#include "TextureManager.h"
#include <numbers>
#include <imgui.h>

void Object3d::Initialize(){
	dxCommon_ = ModelManager::GetInstance()->GetDirectXCommon();
	lightManager_ = LightManager::GetInstance();
	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource = dxCommon_->CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む
	//書き込むためのアドレスを取得
	wvpResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	wvpData->WVP = Matrix4x4::MakeIdentity4x4();
	wvpData->World = Matrix4x4::MakeIdentity4x4();
	wvpData->WorldInverseTranspose = Matrix4x4::MakeIdentity4x4();

	//カメラ用のリソースを作る。今回はcolor1つ分のサイズを用意する
	cameraResource = dxCommon_->CreateBufferResource(sizeof(CameraForGpu));
	//カメラのデータを書き込む
	//書き込むためのアドレスを取得
	cameraResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	cameraData->worldPosition = { 0.0f,0.0f,0.0f };

	//パイプラインを設定
	PipelineState pipelineState;
	pipelineState.shaderName = "Object3d";
	pipelineState.blendMode = blendMode_;
	pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);
	computeShaderPipelineName_ = PipelineManager::GetInstance()->CreateComputePipelineState("SkinningObject3d");
}

void Object3d::Update(){
	if (!isSetWorldMatrix_) {
		worldMatrix_ = Quaternion::MakeAffineMatrix(transform.scale, transform.rotate.ToQuaternion(), transform.translate);
	}
	isSetWorldMatrix_ = false;
	if (parent_) {
		worldMatrix_ *= parent_->GetWorldMatrix();
	}
	Matrix4x4 worldMatrix = worldMatrix_;
	worldMatrix = Quaternion::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3(0.0f, std::numbers::pi_v<float>, 0.0f).ToQuaternion(), {}) * worldMatrix;
	Matrix4x4 worldViewProjectionMatrix = worldMatrix;
	if (CameraManager::GetInstance()->GetCamera()) {
		const Matrix4x4& viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
		worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

		Matrix4x4 cameraWorldMatrix = CameraManager::GetInstance()->GetCamera()->GetWorldMatrix();

		cameraData->worldPosition = {cameraWorldMatrix.m[3][0],cameraWorldMatrix.m[3][1],cameraWorldMatrix.m[3][2]};
	}
	if (!model_) {
		//モデルが設定されていない場合は、ワールド行列だけを更新
		wvpData->WVP = worldViewProjectionMatrix;
		wvpData->World = worldMatrix;
		wvpData->WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(worldMatrix));
		return;
	}
	if (animationData_) {
		animationData_->time += TimeManager::GetInstance()->deltaTime_;
		if (animationData_->isLoop) {
			animationData_->time = fmod(animationData_->time, animationData_->animation->GetDuration());
		} else if (animationData_->time > animationData_->animation->GetDuration()) {
			animationData_->time = animationData_->animation->GetDuration();
		}
		if (nextAnimationData_) {
			nextAnimationData_->time += TimeManager::GetInstance()->deltaTime_;
			skeletonData_->ApplyAnimation(animationData_->animation, nextAnimationData_->animation, nextAnimationData_->time / lerpTime_);
			if (nextAnimationData_->time > lerpTime_) {
				animationData_->animation = nextAnimationData_->animation;
				animationData_->time = 0.0f;
				animationData_->isLoop = nextAnimationData_->isLoop;
				nextAnimationData_.reset(); // 次のアニメーションデータを解放
				skeletonData_->ApplyAnimation(animationData_->animation, animationData_->time);
			}
		} else {
			skeletonData_->ApplyAnimation(animationData_->animation, animationData_->time);
		}
	}
	wvpData->WVP = worldViewProjectionMatrix;
	wvpData->World = worldMatrix;
	wvpData->WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(wvpData->World));

	//スケルトンの更新
	skeletonData_->Update();
	//スキンクラスタの更新
	skinClusterData_->Update(skeletonData_.get());
	for (MaterialData materialData : materialDates_) {
		materialData.material->uvTransform = Matrix4x4::MakeAffineMatrix(
			materialData.uvTransform.scale,
			materialData.uvTransform.rotate,
			materialData.uvTransform.translate);
	}
}

void Object3d::Draw(){
	//モデルが設定されていない場合は何もしない
	if (!model_) {return;}
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	if (!isSkybox_) {
		PipelineManager::GetInstance()->DrawSettingCS(computeShaderPipelineName_);
		for (uint32_t meshIndex = 0; meshIndex < model_->GetMeshData().size(); meshIndex++) {
			skinClusterData_->Draw(meshIndex);
		}
	}
	PipelineManager::GetInstance()->DrawSetting(pipelineStateName_);
	for (uint32_t meshIndex = 0; meshIndex < model_->GetMeshData().size(); meshIndex++) {
		//wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialDates_[meshIndex].materialResource.Get()->GetGPUVirtualAddress());
		//テクスチャを設定
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(materialDates_[meshIndex].textureFilePath_));
		if (!isSkybox_) {
			//カメラCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(4, cameraResource.Get()->GetGPUVirtualAddress());
			//ライトマネージャーのデータを設定
			lightManager_->Draw();
			if (environmentTextureFilePath_ != "") {
				//環境マップテクスチャを設定
				commandList->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetSrvHandleGPU(environmentTextureFilePath_));
			}
			model_->Draw(meshIndex,skinClusterData_->GetSkinnedVerticesView(meshIndex));
		} else {
			model_->Draw(meshIndex);
		}
	}
	//Skeleton
	if (skeletonData_ && isDrawSkeleton_) {
		skeletonData_->Draw(wvpData->World);
	}
}

void Object3d::ImGuiUpdate(const std::string& name){
	std::string groupName = "Object3d";
	if (ImGui::BeginMenu(groupName.c_str())) {
		if (ImGui::CollapsingHeader(name.c_str())) {
			std::string blendName = ":Now Blend";
			std::vector<std::string> blendState{
				"None",      //!< ブレンドなし
				"Normal",    //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
				"Add",       //!< 加算。Src * SrcA + Dest * 1
				"Subtract",  //!< 減算。Dest * 1 - Src * SrcA
				"Multiply",  //!< 乗算。Src * 0 + Dest * Src
				"Screen", };
			int Object3dItem_selected_idx = static_cast<int>(blendMode_);
			const char* currentItem = blendState[Object3dItem_selected_idx].c_str();
			if (ImGui::BeginCombo((name + blendName).c_str(), currentItem)) {
				for (int i = 0; i < blendState.size(); ++i) {
					bool isSelected = (Object3dItem_selected_idx == i);
					if (ImGui::Selectable(blendState[i].c_str(), isSelected)) {
						Object3dItem_selected_idx = i;
						SetBlendMode(static_cast<BlendMode>(i));
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			//Transformの編集
			ImGui::DragFloat3((name + ".Transform.Translate").c_str(), &transform.translate.x, 0.1f);
			ImGui::SliderAngle((name + ".Transform.Rotate.x").c_str(), &transform.rotate.x);
			ImGui::SliderAngle((name + ".Transform.Rotate.y").c_str(), &transform.rotate.y);
			ImGui::SliderAngle((name + ".Transform.Rotate.z").c_str(), &transform.rotate.z);
			ImGui::DragFloat3((name + ".Transform.Scale").c_str(), &transform.scale.x, 0.1f);
			//アニメーションの編集
			ImGui::DragFloat((name + ".LerpTime").c_str(), &lerpTime_, 0.01f, 0.0f);
			//テクスチャのキーを取得
			std::vector<std::string> textureState = TextureManager::GetInstance()->GetKeys();
			uint32_t idIndex = 0;
			ImGui::Indent();
			for (MaterialData& materialData : materialDates_) {
				ImGui::PushID(idIndex);
				if (ImGui::CollapsingHeader((name + "Material").c_str())) {
					//テクスチャ選択
					std::string textureName = name + ": Now Texture";
					std::string textureItem_selected_idx = materialData.textureFilePath_;
					currentItem = textureItem_selected_idx.c_str();
					if (ImGui::BeginCombo((textureName + name).c_str(), currentItem)) {
						for (int i = 0; i < textureState.size(); ++i) {
							bool isSelected = (textureItem_selected_idx == textureState[i]);
							if (ImGui::Selectable(textureState[i].c_str(), isSelected)) {
								materialData.textureFilePath_ = textureState[i];
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					//UVTransformの編集
					ImGui::DragFloat3((name + ".UVTransform.Translate").c_str(), &materialData.uvTransform.translate.x, 0.1f);
					ImGui::SliderAngle((name + ".UVTransform.Rotate.x").c_str(), &materialData.uvTransform.rotate.x);
					ImGui::SliderAngle((name + ".UVTransform.Rotate.y").c_str(), &materialData.uvTransform.rotate.y);
					ImGui::SliderAngle((name + ".UVTransform.Rotate.z").c_str(), &materialData.uvTransform.rotate.z);
					ImGui::DragFloat3((name + ".UVTransform.Scale").c_str(), &materialData.uvTransform.scale.x, 0.1f);
					//マテリアルの編集
					ImGui::ColorEdit4((name + ".Color").c_str(), &materialData.material->color.x);
					ImGui::ColorEdit4((name + ".HighLightColor").c_str(), &materialData.material->highLightColor.x);
					bool isDrawSkeleton = isDrawSkeleton_;
					ImGui::Checkbox((name + ".IsDrawSkeleton").c_str(), &isDrawSkeleton);
					bool enableLighting = (bool)materialData.material->enableLighting;
					ImGui::Checkbox((name + ".EnableLighting").c_str(), &enableLighting);
					materialData.material->enableLighting = enableLighting; // bool型に変換して保存
					isDrawSkeleton_ = isDrawSkeleton; // bool型に変換して保存
					if (environmentTextureFilePath_ != "") {
						bool enableEnvironmentMap = materialData.material->enableEnvironmentMap;
						ImGui::Checkbox((name + ".EnableEnvironmentMap").c_str(), &enableEnvironmentMap);
						materialData.material->enableEnvironmentMap = enableEnvironmentMap; // bool型に変換して保存
					}
					ImGui::DragFloat((name + ".EnvironmentCoefficient").c_str(), &materialData.material->environmentCoefficient, 0.01f, 0.0f, 1.0f);
				}
				ImGui::PopID();
				idIndex++;
			}
			ImGui::Unindent();
		}
		ImGui::EndMenu();
	}
}

Matrix4x4 Object3d::GetJointMatrix(std::string jointName) const {
	return skeletonData_->GetJoints()[skeletonData_->GetJointMap()[jointName]].skeletonSpaceMatrix * Quaternion::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, Vector3(0.0f, std::numbers::pi_v<float>, 0.0f).ToQuaternion(), {});
}

Vector3 Object3d::GetJointsPosition(std::string jointName){
	Vector3 position = { 0.0f,0.0f,0.0f };
	Matrix4x4 jointWorldMatrix = (Matrix4x4)GetJointMatrix(jointName) * worldMatrix_;
	position.x = jointWorldMatrix.m[3][0];
	position.y = jointWorldMatrix.m[3][1];
	position.z = jointWorldMatrix.m[3][2];
	return position;
}

void Object3d::SetWorldMatrix(const Matrix4x4& worldMatrix){
	isSetWorldMatrix_ = true; // WorldMatrixをセットするフラグを立てる
	worldMatrix_ = worldMatrix;
}

void Object3d::SetEnvironmentTexture(const std::string& cubeTextureFilePath) {
	environmentTextureFilePath_ = cubeTextureFilePath;
	for (MaterialData materialData : materialDates_) {
		materialData.material->enableEnvironmentMap = true; // 環境マップを有効にする
	}
}

void Object3d::SetModel(const std::string& filePath){
	//モデルを検索してセット
	model_ = ModelManager::GetInstance()->FindModel(filePath);
	if (filePath == "skybox") {
		//パイプラインを設定
		PipelineState pipelineState;
		pipelineState.shaderName = "Skybox";
		pipelineState.depthMode = DepthMode::kReadOnly; //深度テストを読み取り専用にする
		pipelineState.blendMode = blendMode_;
		pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);
		isSkybox_ = true;
	}
	//スケルトン
	skeletonData_ = std::make_unique<Skeleton>();
	skeletonData_->CreateSkeleton(model_->GetNode());

	//スキンクラスタ
	skinClusterData_ = std::make_unique<SkinCluster>();
	skinClusterData_->CreateSkinCluster(skeletonData_.get(), model_->GetMeshData());
	//マテリアルデータの初期化
	materialDates_.resize(model_->GetMeshData().size());
	for (uint32_t meshIndex = 0; meshIndex < model_->GetMeshData().size();meshIndex++) {
		//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
		materialDates_[meshIndex].materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
		// マップしてポインタ取得
		void* mappedPtr = nullptr;
		materialDates_[meshIndex].materialResource->Map(0, nullptr, &mappedPtr);
		materialDates_[meshIndex].material = reinterpret_cast<Material*>(mappedPtr);

		materialDates_[meshIndex].textureFilePath_ = model_->GetMeshData()[meshIndex].material.textureFilePath;
		//マテリアルデータの初期値を書き込む
		materialDates_[meshIndex].material->color = model_->GetMeshData()[meshIndex].material.color;//色を書き込む
		materialDates_[meshIndex].material->enableLighting = true;//Lightingを有効にする
		materialDates_[meshIndex].material->uvTransform = Matrix4x4::MakeIdentity4x4();//UVTransform単位行列で初期化
		materialDates_[meshIndex].material->shininess = 40.0f;
		materialDates_[meshIndex].material->highLightColor = { 1.0f,1.0f,1.0f,1.0f };
		materialDates_[meshIndex].material->enableEnvironmentMap = false; // 環境マップを無効にする
		materialDates_[meshIndex].material->environmentCoefficient = 1.0f; // 環境マップの寄与度を初期化
	}
}

void Object3d::SetAnimation(const std::string& filePath, bool isLoop){
	//アニメーション
	if (nextAnimationData_) {
		animationData_->animation = nextAnimationData_->animation;
		animationData_->isLoop = nextAnimationData_->isLoop;
		nextAnimationData_->animation = ModelManager::GetInstance()->FindAnimation(filePath);
		nextAnimationData_->time = lerpTime_ - nextAnimationData_->time;
		nextAnimationData_->isLoop = isLoop;
	}else if (animationData_) {
		nextAnimationData_ = std::make_unique<AnimationData>();
		nextAnimationData_->animation = ModelManager::GetInstance()->FindAnimation(filePath);
		nextAnimationData_->time = 0.0f;
		nextAnimationData_->isLoop = isLoop;
	} else {
		animationData_ = std::make_unique<AnimationData>();
		animationData_->animation = ModelManager::GetInstance()->FindAnimation(filePath);
		animationData_->time = 0.0f;
		animationData_->isLoop = isLoop;
	}
}

void Object3d::SetBlendMode(const BlendMode& blendMode){
	blendMode_ = blendMode;
	PipelineState pipelineState;
	if (isSkybox_) {
		pipelineState.shaderName = "Skybox";
		pipelineState.depthMode = DepthMode::kReadOnly;
	} else {
		pipelineState.shaderName = "Object3d";
	}
	pipelineState.blendMode = blendMode_;
	pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);
}

Vector3 Object3d::GetCenterPosition() const
{
	Vector3 worldPos;
	//ワールド行列の平行移動成分取得
	worldPos.x = worldMatrix_.m[3][0];
	worldPos.y = worldMatrix_.m[3][1];
	worldPos.z = worldMatrix_.m[3][2];

	return worldPos;
}