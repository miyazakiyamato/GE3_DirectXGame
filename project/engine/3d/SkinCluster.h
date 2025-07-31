#pragma once
#include <span>
#include "Model.h"

class SrvUavManager;
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
	struct VertexForGPU {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct SkinningInformationForGPU {
		uint32_t numVertices;
	};
	struct SkinClusterData {
		// Influence
		Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
		std::span<VertexInfluence> mappedInfluence;
		uint32_t influenceSrvIndex = 0;

		// 元頂点 (Input)
		Microsoft::WRL::ComPtr<ID3D12Resource> inputVerticesResource;
		uint32_t inputVertexSrvIndex = 0;

		// スキニング後頂点 (Output)
		Microsoft::WRL::ComPtr<ID3D12Resource> skinnedVerticesResource;
		D3D12_VERTEX_BUFFER_VIEW skinnedVerticesView;
		uint32_t outputVertexUavIndex = 0;

		// 定数バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> skinningInformationResource;
		SkinningInformationForGPU* mappedSkinningInformation = nullptr;
	};
public://メンバ関数
	//SkinClusterの生成
	void CreateSkinCluster(Skeleton* skeleton, std::vector<Model::Mesh> modelData);
	//更新
	void Update(Skeleton* skeleton);
	//描画
	void Draw(size_t meshIndex);

private://メンバ変数
	SrvUavManager* srvUavManager_ = nullptr;//SRVやUAVを管理するクラスへのポインタ

	std::vector<SkinClusterData> skinClusterDates_;

	std::vector<Matrix4x4> inverseBindPoseMatrices_;
	ComPtr<ID3D12Resource> paletteResource_;
	std::span<WellForGPU> mappedPalette_;
	uint32_t paletteSrvIndex_ = 0;
public:
	const D3D12_VERTEX_BUFFER_VIEW* GetSkinnedVerticesView(size_t meshIndex) const {
		return &skinClusterDates_[meshIndex].skinnedVerticesView;
	}
};

