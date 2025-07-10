#include "SkinCluster.h"
#include "ModelManager.h"
#include "SrvManager.h"
#include "Skeleton.h"

void SkinCluster::CreateSkinCluster(Skeleton* skeleton,std::vector<Model::Mesh> modelData) {
	//palette用のリソースを確保
	paletteResource_ = ModelManager::GetInstance()->GetDirectXCommon()->CreateBufferResource(sizeof(WellForGPU) * (UINT)skeleton->GetJoints().size());
	WellForGPU* palette = nullptr;
	paletteResource_->Map(0, nullptr, reinterpret_cast<void**>(&palette));
	mappedPalette_ = {palette, skeleton->GetJoints().size()};

	//paletteのSRVを生成
	uint32_t srvIndex = ModelManager::GetInstance()->GetSrvManager()->ALLocate();
	ModelManager::GetInstance()->GetSrvManager()->CreateSRVforStructuredBuffer(srvIndex, paletteResource_.Get(), (UINT)skeleton->GetJoints().size(), sizeof(WellForGPU));
	paletteSrvHandle_.first = ModelManager::GetInstance()->GetSrvManager()->GetCPUDescriptorHandle(srvIndex);
	paletteSrvHandle_.second = ModelManager::GetInstance()->GetSrvManager()->GetGPUDescriptorHandle(srvIndex);

	skinClusterDates_.resize(modelData.size());//モデルのメッシュ数分だけSkinClusterDataを確保
	for (uint32_t meshIndex = 0; meshIndex < modelData.size(); meshIndex++) {
		//influence用のリソースを確保
		skinClusterDates_[meshIndex].influenceResource = ModelManager::GetInstance()->GetDirectXCommon()->CreateBufferResource(sizeof(VertexInfluence) * (UINT)modelData[meshIndex].vertices.size());
		VertexInfluence* mappedInfluence = nullptr;
		skinClusterDates_[meshIndex].influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * (UINT)modelData[meshIndex].vertices.size());
		skinClusterDates_[meshIndex].mappedInfluence = { mappedInfluence, modelData[meshIndex].vertices.size() };

		//influenceのVBVを生成
		skinClusterDates_[meshIndex].influenceBufferView.BufferLocation = skinClusterDates_[meshIndex].influenceResource->GetGPUVirtualAddress();
		skinClusterDates_[meshIndex].influenceBufferView.SizeInBytes = sizeof(VertexInfluence) * (UINT)modelData[meshIndex].vertices.size();
		skinClusterDates_[meshIndex].influenceBufferView.StrideInBytes = sizeof(VertexInfluence);
	}
	

	//inverseBindPoseMatrixを格納する場所を作成
	inverseBindPoseMatrices_.resize(skeleton->GetJoints().size());
	std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), []() {return Matrix4x4::MakeIdentity4x4();});

	//modelのskinClusterDataを解析
	for (uint32_t meshIndex = 0; meshIndex < modelData.size(); meshIndex++) {
		for (const auto& jointWeight : modelData[meshIndex].skinClusterData) {
			auto it = skeleton->GetJointMap().find(jointWeight.first);
			if (it == skeleton->GetJointMap().end()) {//Jointが見つからなかった場合
				continue;
			}
			inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
			for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
				auto& currentInfluence = skinClusterDates_[meshIndex].mappedInfluence[vertexWeight.vertexIndex];
				for (uint32_t index = 0; index < kNumMaxInfluence; index++) {
					if (currentInfluence.weights[index] == 0.0f) {
						currentInfluence.weights[index] = vertexWeight.weight;
						currentInfluence.jointIndices[index] = (*it).second;
						break;
					}
				}
			}
		}
		if (modelData[meshIndex].skinClusterData.empty() || skinClusterDates_[meshIndex].mappedInfluence.empty()) {
			for (size_t i = 0; i < modelData[meshIndex].vertices.size(); ++i) {
				skinClusterDates_[meshIndex].mappedInfluence[i].weights[0] = 1.0f;
				skinClusterDates_[meshIndex].mappedInfluence[i].jointIndices[0] = 0; // ジョイント0
			}
		}
	}
}

void SkinCluster::Update(Skeleton* skeleton){
	for (size_t jointIndex = 0; jointIndex < skeleton->GetJoints().size(); ++jointIndex) {
		assert(jointIndex < inverseBindPoseMatrices_.size());
		mappedPalette_[jointIndex].skeletonSpaceMatrix = inverseBindPoseMatrices_[jointIndex] * skeleton->GetJoints()[jointIndex].skeletonSpaceMatrix;
		mappedPalette_[jointIndex].skeletonSpaceInverseTransposeMatrix = Matrix4x4::Transpose(Matrix4x4::Inverse(mappedPalette_[jointIndex].skeletonSpaceMatrix));
	}
}

void SkinCluster::Draw(size_t meshIndex){
	ID3D12GraphicsCommandList* commandList = ModelManager::GetInstance()->GetDirectXCommon()->GetCommandList();
	commandList->IASetVertexBuffers(1, 1, &skinClusterDates_[meshIndex].influenceBufferView);//VBVを設定
	commandList->SetGraphicsRootDescriptorTable(8, paletteSrvHandle_.second);
}
