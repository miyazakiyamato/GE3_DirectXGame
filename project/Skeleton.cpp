#include "Skeleton.h"

void Skeleton::CreateSkeleton(const Model::Node& rootNode){
	root = CreateJoint(rootNode, {}, joints);

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : joints) {
		jointMap.emplace(joint.name, joint.index);
	}

	//全てのJointを更新。
	for (Joint& joint : joints) {
		joint.localMatrix = Quaternion::MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * joints[*joint.parent].skeletonSpaceMatrix;
		} else {
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

int32_t Skeleton::CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints){
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = Matrix4x4::MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	joints.push_back(joint);
	for (const Model::Node& child : node.children) {
		//子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	return joint.index;
}
