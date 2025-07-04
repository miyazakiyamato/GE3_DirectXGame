#include "Object3d.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "PipelineManager.h"
#include "TimeManager.h"
#include "Line3D.h"
#include "TextureManager.h"
#include <numbers>

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

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

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
}

void Object3d::Update(){
	Matrix4x4 worldMatrix;
	Matrix4x4 worldViewProjectionMatrix;
	worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate + Vector3(0.0f,std::numbers::pi_v<float>,0.0f), transform.translate);
	if (parent_) {
		worldMatrix = worldMatrix * parent_->GetWorldMatrix();
	}
	if (CameraManager::GetInstance()->GetCamera()) {
		const Matrix4x4& viewProjectionMatrix = CameraManager::GetInstance()->GetCamera()->GetViewProjectionMatrix();
		worldViewProjectionMatrix = worldMatrix * viewProjectionMatrix;

		Matrix4x4 cameraWorldMatrix = CameraManager::GetInstance()->GetCamera()->GetWorldMatrix();

		cameraData->worldPosition = {cameraWorldMatrix.m[3][0],cameraWorldMatrix.m[3][1],cameraWorldMatrix.m[3][2]};
	}
	else {
		worldViewProjectionMatrix = worldMatrix;
	}
	if (model_) {
		if (animationData_) {
			animationData_->time += TimeManager::GetInstance()->deltaTime_;
			if (animationData_->isLoop) {
				animationData_->time = fmod(animationData_->time, animationData_->animation->GetDuration());
			} else if (animationData_->time > animationData_->animation->GetDuration()) {
				animationData_->time = animationData_->animation->GetDuration();
			}
			/*Matrix4x4 localMatrix = animationData_->animation->MakeLocalMatrix(model_->GetModelData().rootNode.name,animationData_->time);
			wvpData->WVP = localMatrix * worldViewProjectionMatrix;
			wvpData->World = localMatrix * worldMatrix;
			wvpData->WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(wvpData->World));*/

			wvpData->WVP = worldViewProjectionMatrix;
			wvpData->World = worldMatrix;
			wvpData->WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(wvpData->World));

			//スケルトンの更新
			if (skeletonData_) {
				skeletonData_->ApplyAnimation(animationData_->animation, animationData_->time);
				skeletonData_->Update();

				//スキンクラスタの更新
				if (skinClusterData_) {
					skinClusterData_->Update(skeletonData_.get());
				}
			}
		}
		else {
			wvpData->WVP = (Matrix4x4)model_->GetModelData().rootNode.localMatrix * worldViewProjectionMatrix;
			wvpData->World = (Matrix4x4)model_->GetModelData().rootNode.localMatrix * worldMatrix;
			wvpData->WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(wvpData->World));
		}
	} else {
		wvpData->WVP = worldViewProjectionMatrix;
		wvpData->World = worldMatrix;
		wvpData->WorldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(worldMatrix));
	}
}

void Object3d::Draw(){

	if (model_) {
		// コマンドリストの取得
		ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
		
		PipelineManager::GetInstance()->DrawSetting(pipelineStateName_);
		
		if (skinClusterData_) {
			skinClusterData_->Draw();
		} 
		//マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
		//wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, wvpResource.Get()->GetGPUVirtualAddress());
		//テクスチャを設定
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath_));
		if (!isSkybox_) {
			//カメラCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(4, cameraResource.Get()->GetGPUVirtualAddress());
			//ライトマネージャーのデータを設定
			lightManager_->Draw();
			if (materialData->enableEnvironmentMap) {
				//環境マップテクスチャを設定
				commandList->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetSrvHandleGPU(environmentTextureFilePath_));
			}
		}

		model_->Draw();

		//Skeleton
		if (skeletonData_ && isDrawSkeleton_) {
			skeletonData_->Draw(wvpData->World);
		}
	}

}

void Object3d::SetEnvironmentTexture(const std::string& cubeTextureFilePath) {
	environmentTextureFilePath_ = cubeTextureFilePath;
	materialData->enableEnvironmentMap = true; // 環境マップを有効にする
}

void Object3d::SetModel(const std::string& filePath){
	//モデルを検索してセット
	model_ = ModelManager::GetInstance()->FindModel(filePath);
	textureFilePath_ = model_->GetModelData().material.textureFilePath;
	if (filePath == "skybox") {
		//パイプラインを設定
		PipelineState pipelineState;
		pipelineState.shaderName = "Skybox";
		pipelineState.depthMode = DepthMode::kReadOnly; //深度テストを読み取り専用にする
		pipelineState.blendMode = blendMode_;
		pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);
		isSkybox_ = true;
	}
	//マテリアルデータの初期値を書き込む
	materialData->color = model_->LoadColor();//色を書き込む
	materialData->enableLighting = true;//Lightingを有効にする
	materialData->uvTransform = Matrix4x4::MakeIdentity4x4();//UVTransform単位行列で初期化
	materialData->shininess = 40.0f;
	materialData->highLightColor = { 1.0f,1.0f,1.0f,1.0f };
	materialData->enableEnvironmentMap = false; // 環境マップを無効にする
	materialData->environmentCoefficient = 1.0f; // 環境マップの寄与度を初期化
}

void Object3d::SetAnimation(const std::string& filePath, bool isLoop){
	//アニメーション
	animationData_ = std::make_unique<AnimationData>();
	animationData_->animation = ModelManager::GetInstance()->FindAnimation(filePath);
	animationData_->time = 0.0f;
	animationData_->isLoop = isLoop;
	//スケルトン
	skeletonData_ = std::make_unique<Skeleton>();
	skeletonData_->CreateSkeleton(model_->GetModelData().rootNode);

	//スキンクラスタ
	skinClusterData_ = std::make_unique<SkinCluster>();
	skinClusterData_->CreateSkinCluster(skeletonData_.get(), model_->GetModelData());

	//パイプラインを設定
	PipelineState pipelineState;
	pipelineState.shaderName = "SkinningObject3d";
	pipelineState.blendMode = blendMode_;
	pipelineStateName_ = PipelineManager::GetInstance()->CreatePipelineState(pipelineState);
}

void Object3d::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
	PipelineState pipelineState;
	if (skinClusterData_) {
		pipelineState.shaderName = "SkinningObject3d";
	} else if (isSkybox_) {
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
	worldPos.x = wvpData->World.m[3][0];
	worldPos.y = wvpData->World.m[3][1];
	worldPos.z = wvpData->World.m[3][2];

	return worldPos;
}

