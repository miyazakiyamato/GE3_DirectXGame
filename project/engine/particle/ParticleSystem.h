#pragma once
#include <string>
#include <unordered_map>
#include "BaseParticleEmitter.h"

class ParticleSystem{
public:
	// ParticleSystemのコンストラクタ
	ParticleSystem() {};
	void Finalize();
	// パーティクルの更新
	void Update();
	// パーティクルの描画
	void Draw();
	// パーティクルエミッターのセット
	void SetParticleEmitter(std::unique_ptr<BaseParticleEmitter> emitter);
	//パーティクルの管理取得
	BaseParticleEmitter* GetParticleEmitter(const std::string& emitterName);
	// パーティクルの発生
	void Emit(const std::string& emitterName);
	// 調整項目の更新
	void UpdateGlobalVariables();
	BaseParticleEmitter* FindEmitter(const std::string& emitterName) {return emitters_[emitterName].get();}
private:
		// ローカル関数
	// 調整項目の初期化
	void InitializeGlobalVariables();
	// 調整項目の適用
	void ApplyGlobalVariables();
		// メンバ変数
	// パーティクルエミッターの管理
	std::unordered_map<std::string, std::unique_ptr<BaseParticleEmitter>> emitters_;
};

