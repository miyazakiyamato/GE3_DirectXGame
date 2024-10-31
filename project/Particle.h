#pragma once
#include "d3dx12.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class Particle {
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバ関数
	//初期化
	void Initialize(std::string textureFilePath);
	//更新
	void Update();
	//描画
	void Draw();
private://ローカル関数
private://メンバ変数
	const float kDeltaTime_ = 1.0f / 60.0f;

	Transform transform_{};
	Vector3 velocity_{};
	Vector4 color_;
	float lifeTime_;
	float currentTime_;

	bool isAlive_ = true;
public://ゲッターセッター
    // 生存状態チェック
    bool GetIsAlive() const { return isAlive_; }

    // 各種ゲッター
    const Vector3& GetPosition() const { return transform_.translate; }
    const Vector3& GetScale() const { return transform_.scale; }
    const Vector3& GetRotation() const { return transform_.rotate; }
    const Vector4& GetColor() const { return color_; }
    float GetLifeTime() const { return lifeTime_; }

    // 各種セッター
    void SetPosition(const Vector3& position) { transform_.translate = position; }
    void SetScale(const Vector3& scale) { transform_.scale = scale; }
    void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
    void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
    void SetColor(const Vector4& color) { color_ = color; }
    void SetLifeTime(float lifeTime) { lifeTime_ = lifeTime; }

    // ワールド行列の取得
    Matrix4x4 GetWorldMatrix() const;

};
