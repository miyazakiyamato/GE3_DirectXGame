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
public://メンバ関数
	//SkinClusterの生成
	void CreateSkinCluster(Skeleton* skeleton, const Model::ModelData& modelData);
	//更新
	void Update(Skeleton* skeleton);
	//描画
	void Draw();
private://メンバ変数
	std::vector<Matrix4x4> inverseBindPoseMatrices_;

	ComPtr<ID3D12Resource> influenceResource_;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView_;
	std::span<VertexInfluence> mappedInfluence_;

	ComPtr<ID3D12Resource> paletteResource_;
	std::span<WellForGPU> mappedPalette_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle_;
public://ゲッターセッター
	const D3D12_VERTEX_BUFFER_VIEW& GetInfluenceBufferView() const { return influenceBufferView_; }
};

