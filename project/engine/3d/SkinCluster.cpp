#include "SkinCluster.h"
#include "ModelManager.h"
#include "SrvManager.h"
#include "Skeleton.h"

void SkinCluster::CreateSkinCluster(Skeleton* skeleton, const Model::ModelData& modelData) {
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

	//influence用のリソースを確保
	influenceResource_ = ModelManager::GetInstance()->GetDirectXCommon()->CreateBufferResource(sizeof(VertexInfluence) * (UINT)modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	influenceResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * (UINT)modelData.vertices.size());
	mappedInfluence_ = { mappedInfluence, modelData.vertices.size() };

	//influenceのVBVを生成
	influenceBufferView_.BufferLocation = influenceResource_->GetGPUVirtualAddress();
	influenceBufferView_.SizeInBytes = sizeof(VertexInfluence) * (UINT)modelData.vertices.size();
	influenceBufferView_.StrideInBytes = sizeof(VertexInfluence);

	//inverseBindPoseMatrixを格納する場所を作成
	inverseBindPoseMatrices_.resize(skeleton->GetJoints().size());
	std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), []() {return Matrix4x4::MakeIdentity4x4();});

	//modelのskinClusterDataを解析
	for (const auto& jointWeight : modelData.skinClusterData) {
		auto it = skeleton->GetJointMap().find(jointWeight.first);
		if (it == skeleton->GetJointMap().end()) {//Jointが見つからなかった場合
			continue;
		}
		inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence; index++) {
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
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

void SkinCluster::Draw(){
	ID3D12GraphicsCommandList* commandList = ModelManager::GetInstance()->GetDirectXCommon()->GetCommandList();
	commandList->IASetVertexBuffers(1, 1, &influenceBufferView_);//VBVを設定
	commandList->SetGraphicsRootDescriptorTable(7, paletteSrvHandle_.second);
}
