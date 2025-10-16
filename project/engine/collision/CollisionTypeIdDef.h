#pragma once

#include <cstdint>

//コリジョン種別ID定義
enum class CollisionTypeIdDef : uint32_t {
	kDefault,
	kFather,
	kChild,
	kEnemy,
	kTorch,
	kTree,
	kOasis,
};