#include "Skeleton.h"
#include "Animation.h"
#include "Line3D.h"

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

void Skeleton::ApplyAnimation(Animation* animation, float time){
	for (Skeleton::Joint& joint : joints) {
		//対象のJointのAnimationがあれば、値の適用を行う
		if (auto it = animation->GetNodeAnimationsMap().find(joint.name); it != animation->GetNodeAnimationsMap().end()) {
			const Animation::NodeAnimation& rootNodeAnimation = (*it).second;
			joint.localMatrix = animation->MakeLocalMatrix(joint.name, time);
		}
	}
}

void Skeleton::Update(){
	//skeletonSpaceMatrixを更新。
	for (Joint& joint : joints) {
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * joints[*joint.parent].skeletonSpaceMatrix;
		} else {
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void Skeleton::Draw(Matrix4x4 worldMatrix){
	for (Skeleton::Joint& joint : joints) {
		//Line3dManager::GetInstance()->DrawSphere({ Matrix4x4::Transform({0.0f,0.0f,0.0f}, joint.skeletonSpaceMatrix * wvpData->World), 0.01f }, { 1.0f, 1.0f, 1.0f, 1.0f },2);
		Line3dManager::GetInstance()->DrawSphere(joint.skeletonSpaceMatrix * worldMatrix, { 1.0f, 1.0f, 1.0f, 1.0f }, 4);
		if (joint.parent && *joint.parent < joints.size()) {
			Line3dManager::GetInstance()->DrawLine(Matrix4x4::Transform({ 0.0f,0.0f,0.0f }, joint.skeletonSpaceMatrix * worldMatrix), Matrix4x4::Transform({ 0.0f,0.0f,0.0f }, joints[*joint.parent].skeletonSpaceMatrix * worldMatrix), { 1.0f, 1.0f, 1.0f, 1.0f });
		}
	}
}
