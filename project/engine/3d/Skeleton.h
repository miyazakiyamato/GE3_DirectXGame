#pragma once
#include <optional>
#include <map>
#include "Model.h"

class Skeleton{
public:
	struct Joint {
		Model::QuaternionTransform transform;
		Matrix4x4 localMatrix;
		Matrix4x4 skeletonSpaceMatrix;
		std::string name;
		std::vector<int32_t> children;//子JointのIndexのリスト
		int32_t index;//自身のindex
		std::optional<int32_t> parent;//親JointのIndex
	};
public:
	//Skeletonをつくる
	void CreateSkeleton(const Model::Node& rootNode);
	//NodeからJointをつくる
	int32_t CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
private:
	int32_t root;//RootJointのIndex
	std::map<std::string, int32_t> jointMap;//Joint名とIndexとの辞書
	std::vector<Joint> joints;//所属しているジョイント
public:
	std::vector<Joint>& GetJoints() { return joints; }
};

