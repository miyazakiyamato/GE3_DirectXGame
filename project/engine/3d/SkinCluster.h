#pragma once
#include <span>
#include "Model.h"

class Skeleton;
class SkinCluster{
private:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://構造体
	static const uint32_t kNumMaxInfluence = 4;//1頂点あたりの影響を受けるJointの最大数
	struct VertexInfluence {
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointIndices;
	};
	struct WellForGPU {
		Matrix4x4 skeletonSpaceMatrix;//位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線用
	};
	struct SkinClusterData{
		ComPtr<ID3D12Resource> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;
	};
public://メンバ関数
	//SkinClusterの生成
	void CreateSkinCluster(Skeleton* skeleton, std::vector<Model::Mesh> modelData);
	//更新
	void Update(Skeleton* skeleton);
	//描画
	void Draw(size_t meshIndex);
private://メンバ変数
	std::vector<Matrix4x4> inverseBindPoseMatrices_;

	std::vector<SkinClusterData> skinClusterDates_;//SkinClusterのデータを格納する

	ComPtr<ID3D12Resource> paletteResource_;
	std::span<WellForGPU> mappedPalette_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle_;
public://ゲッターセッター
	const std::vector<SkinClusterData>& GetSkinClusterData() const { return skinClusterDates_; }
};

