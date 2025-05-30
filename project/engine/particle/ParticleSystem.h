#pragma once
#include <string>
#include <unordered_map>
#include "BaseParticleEmitter.h"

class ParticleSystem{
public:
	// ParticleSystemのコンストラクタ
	ParticleSystem() {};
	// パーティクルの更新
	void Update();
	// パーティクルの描画
	void Draw();
	// パーティクルの生成
	void CreateParticleEmitter(const std::string& emitterName, std::unique_ptr<BaseParticleEmitter> emitter);
	//パーティクルの発生
	void Emit(const std::string& emitterName);
private:
	// パーティクルエミッターの管理
	std::unordered_map<std::string, std::unique_ptr<BaseParticleEmitter>> emitters_;
};

