#pragma once
#include <map>
#include "Quaternion.h"

class Animation{
public://構造体
	//アニメーションデータ
	template <typename tValue>
	struct Keyframe {
		float time; //!< キーフレームの時間
		tValue value; //!< キーフレームの値
	};

	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	template <typename tValue>
	struct AnimationCurve {
		std::vector<Keyframe<tValue>> keyframes;
	};
	struct NodeAnimation {
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3> scale;
	};
public://メンバ関数
	//アニメーションを読み込む
	void LoadFile(const std::string& filename);

	Vector3 CalculateValue(const AnimationCurve<Vector3>& curve, float time);
	Quaternion CalculateValue(const AnimationCurve<Quaternion>& curve, float time);

	Matrix4x4 MakeLocalMatrix(std::string rootNodeName,float time);
private://メンバ変数
	std::string directoryPath = "./resources/model/";//!<ディレクトリパス

	float duration;//!<アニメーションの長さ
	std::map<std::string, NodeAnimation> nodeAnimations;//!<ノードアニメーション
public://ゲッターセッター
	float GetDuration() { return duration; }
	const NodeAnimation& GetNodeAnimations(std::string rootNodeName) { return nodeAnimations[rootNodeName]; }
	const std::map<std::string, NodeAnimation>& GetNodeAnimationsMap() { return nodeAnimations; }
};

