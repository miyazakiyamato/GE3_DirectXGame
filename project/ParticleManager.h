#pragma once
#include <map>
#include "ParticleCommon.h"

class ParticleManager {
public://メンバ関数
	//シングルトンインスタンスの取得
	static ParticleManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//終了
	void Finalize();

	//共通描画設定
	void DrawCommonSetting();
	//
	void ChangeBlendMode(ParticleCommon::BlendMode blendMode);

private://シングルインスタンス
	static ParticleManager* instance;

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;
private://メンバ変数
	//モデルデータ
	//std::map<std::string, std::unique_ptr<Model>> models;

	std::unique_ptr<ParticleCommon> particleCommon_ = nullptr;
public://ゲッターセッター
	ParticleCommon* GetParticleCommon() const { return particleCommon_.get(); }
};
