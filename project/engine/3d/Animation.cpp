#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <cassert>

void Animation::LoadFile(const std::string& filename){
	Assimp::Importer importer;
	std::string filePath = directoryPath + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);//アニメーションがないのは対応しない
	aiAnimation* animationAssimp = scene->mAnimations[0];//最初のアニメーションを取得
	duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//アニメーションの長さを取得

	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		//位置情報
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		//回転情報
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		//スケール情報
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}
}

Vector3 Animation::CalculateValue(const AnimationCurve<Vector3>& curve, float time){
	assert(!curve.keyframes.empty());//キーフレームがないのは対応しない
	if (curve.keyframes.size() == 1 || time <= curve.keyframes[0].time) {
		return curve.keyframes[0].value;
	}
	for (size_t index = 0; index < curve.keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (curve.keyframes[index].time <= time && time <= curve.keyframes[nextIndex].time) {
			float t = (time - curve.keyframes[index].time) / (curve.keyframes[nextIndex].time - curve.keyframes[index].time);
			return Vector3::Lerp(curve.keyframes[index].value, curve.keyframes[nextIndex].value, t);
		}
	}
	return (*curve.keyframes.rbegin()).value;
}
Quaternion Animation::CalculateValue(const AnimationCurve<Quaternion>& curve, float time) {
	assert(!curve.keyframes.empty());//キーフレームがないのは対応しない
	if (curve.keyframes.size() == 1 || time <= curve.keyframes[0].time) {
		return curve.keyframes[0].value;
	}
	for (size_t index = 0; index < curve.keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (curve.keyframes[index].time <= time && time <= curve.keyframes[nextIndex].time) {
			float t = (time - curve.keyframes[index].time) / (curve.keyframes[nextIndex].time - curve.keyframes[index].time);
			return Quaternion::Slerp(curve.keyframes[index].value, curve.keyframes[nextIndex].value, t);
		}
	}
	return (*curve.keyframes.rbegin()).value;
}

Matrix4x4 Animation::MakeLocalMatrix(std::string rootNodeName,float time){
	Matrix4x4 localMatrix;
	NodeAnimation& rootNodeAnimation = nodeAnimations[rootNodeName];
	Vector3 translate = CalculateValue(rootNodeAnimation.translate, time);
	Quaternion rotate = CalculateValue(rootNodeAnimation.rotate, time);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale, time);
	localMatrix = Quaternion::MakeAffineMatrix(scale, rotate, translate);

	return localMatrix;
}
