#pragma once
#include "Collider.h"
#include "Object3d.h"

class BaseCharacter : public Collider{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

protected:
	//ワールドデータ
	std::unique_ptr<Object3d> baseObject3D_ = nullptr;

public:
	virtual Vector3 GetCenterPosition() const override;
};

