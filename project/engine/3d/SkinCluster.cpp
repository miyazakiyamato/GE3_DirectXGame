#include "SkinCluster.h"
#include "ModelManager.h"
#include "SrvUavManager.h"
#include "Skeleton.h"

void SkinCluster::CreateSkinCluster(Skeleton* skeleton,std::vector<Model::Mesh> modelData) {
    DirectXCommon* dxCommon = ModelManager::GetInstance()->GetDirectXCommon();
	srvUavManager_ = ModelManager::GetInstance()->GetSrvManager();
    const size_t jointSize = skeleton->GetJoints().size();

    // --- 1. 全メッシュ共通のリソースを作成 ---
    // 行列パレット (SRV)
    paletteResource_ = dxCommon->CreateBufferResource(sizeof(WellForGPU) * jointSize);
    WellForGPU* paletteData = nullptr;
    paletteResource_->Map(0, nullptr, reinterpret_cast<void**>(&paletteData));
    mappedPalette_ = { paletteData, jointSize };

    paletteSrvIndex_ = srvUavManager_->Allocate();
    srvUavManager_->CreateSRVforStructuredBuffer(paletteSrvIndex_, paletteResource_.Get(), (UINT)jointSize, sizeof(WellForGPU));

    // --- 2. メッシュごとのリソースを作成 ---
    skinClusterDates_.resize(modelData.size());
    for (uint32_t meshIndex = 0; meshIndex < modelData.size(); ++meshIndex) {
        SkinClusterData& data = skinClusterDates_[meshIndex];
        const Model::Mesh& mesh = modelData[meshIndex];
        const size_t vertexCount = mesh.vertices.size();

        // Influence情報 (SRV)
        data.influenceResource = dxCommon->CreateBufferResource(sizeof(VertexInfluence) * vertexCount);
        VertexInfluence* mappedInfluenceData = nullptr;
        data.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluenceData));
        data.mappedInfluence = { mappedInfluenceData, vertexCount };
        // マップしたメモリを0で初期化
        std::memset(mappedInfluenceData, 0, sizeof(VertexInfluence) * vertexCount);

        data.influenceSrvIndex = srvUavManager_->Allocate();
        srvUavManager_->CreateSRVforStructuredBuffer(data.influenceSrvIndex, data.influenceResource.Get(), (UINT)vertexCount, sizeof(VertexInfluence));

        // CSへの入力となる元の頂点データ (SRV)
        data.inputVerticesResource = dxCommon->CreateBufferResource(sizeof(VertexForGPU) * vertexCount);
        // 元の頂点データをコピー
        VertexForGPU* mappedInputData = nullptr;
        data.inputVerticesResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInputData));
        std::memcpy(mappedInputData, mesh.vertices.data(), sizeof(VertexForGPU) * vertexCount);
        data.inputVerticesResource->Unmap(0, nullptr); // コピーしたらアンマップしてよい

        data.inputVertexSrvIndex = srvUavManager_->Allocate();
        srvUavManager_->CreateSRVforStructuredBuffer(data.inputVertexSrvIndex, data.inputVerticesResource.Get(), (UINT)vertexCount, sizeof(VertexForGPU));

        // CSからの出力先となる頂点データ (UAV)
        data.skinnedVerticesResource = dxCommon->CreateRWBufferResource(sizeof(VertexForGPU) * vertexCount);
        data.outputVertexUavIndex = srvUavManager_->Allocate();
        // SrvManagerにUAV作成用の関数がある前提
        srvUavManager_->CreateUAVforStructuredBuffer(data.outputVertexUavIndex, data.skinnedVerticesResource.Get(), (UINT)vertexCount, sizeof(VertexForGPU));

        // この出力リソースを描画で使うための頂点バッファビューも作成しておく
        data.skinnedVerticesView.BufferLocation = data.skinnedVerticesResource->GetGPUVirtualAddress();
        data.skinnedVerticesView.SizeInBytes = (UINT)(sizeof(VertexForGPU) * vertexCount);
        data.skinnedVerticesView.StrideInBytes = sizeof(VertexForGPU);

        // スキニング情報 (定数バッファ)
        data.skinningInformationResource = dxCommon->CreateBufferResource(sizeof(SkinningInformationForGPU));
        data.skinningInformationResource->Map(0, nullptr, reinterpret_cast<void**>(&data.mappedSkinningInformation));
        data.mappedSkinningInformation->numVertices = (UINT)vertexCount; // CSに頂点数を渡す
    }

    // --- 3. CPU側でInfluenceデータを計算してリソースに書き込む ---
    inverseBindPoseMatrices_.resize(jointSize);
    std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), [] { return Matrix4x4::MakeIdentity4x4(); });

    for (uint32_t meshIndex = 0; meshIndex < modelData.size(); ++meshIndex) {
        auto& targetMappedInfluence = skinClusterDates_[meshIndex].mappedInfluence;

        for (const auto& jointWeight : modelData[meshIndex].skinClusterData) {
            auto it = skeleton->GetJointMap().find(jointWeight.first);
            if (it == skeleton->GetJointMap().end()) {continue;}

            inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
            for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
                auto& currentInfluence = targetMappedInfluence[vertexWeight.vertexIndex];
                for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
                    if (currentInfluence.weights[index] == 0.0f) {
                        currentInfluence.weights[index] = vertexWeight.weight;
                        currentInfluence.jointIndices[index] = (*it).second;
                        break;
                    }
                }
            }
        }

        // ボーンがない場合のデフォルトウェイト設定
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
	// パイプラインを設定
	commandList->SetComputeRootConstantBufferView(0, skinClusterDates_[meshIndex].skinningInformationResource.Get()->GetGPUVirtualAddress());
	srvUavManager_->SetComputeRootDescriptorTable(1, paletteSrvIndex_);
	srvUavManager_->SetComputeRootDescriptorTable(2, skinClusterDates_[meshIndex].inputVertexSrvIndex);
	srvUavManager_->SetComputeRootDescriptorTable(3, skinClusterDates_[meshIndex].influenceSrvIndex);
	srvUavManager_->SetComputeRootDescriptorTable(4, skinClusterDates_[meshIndex].outputVertexUavIndex);

    commandList->Dispatch(UINT(skinClusterDates_[meshIndex].mappedSkinningInformation->numVertices + 1023 / 1024), 1, 1);
}
