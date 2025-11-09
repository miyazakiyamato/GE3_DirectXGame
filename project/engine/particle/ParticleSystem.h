#pragma once
#include <string>
#include <unordered_map>
#include "BaseParticleEmitter.h"

class GlobalVariables;
class ParticleSystem{
public:
	// ParticleSystemのコンストラクタ
	ParticleSystem() {};
	void Finalize();
	// パーティクルの初期化
	void Initialize();
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
	// ImGuiの更新
	void ImGuiUpdate();
	BaseParticleEmitter* FindEmitter(const std::string& emitterName) {return emitters_[emitterName].get();}
private:
		// ローカル関数
	std::unique_ptr<BaseParticleEmitter> CreateEmitterByType(const std::string& typeName);
	// 調整項目の初期化
	void InitializeGlobalVariables();
	// 調整項目の適用
	void ApplyGlobalVariables();
		// メンバ変数
	// ポインタ
	GlobalVariables* globalVariables_ = nullptr;
	// パーティクルエミッターの管理
	std::unordered_map<std::string, std::unique_ptr<BaseParticleEmitter>> emitters_;
	//  エミッター名入力用テキスト
	std::string emitterNameText_ = ""; // エミッター名
	char textBuffer_[128] = ""; // 入力用のバッファ
	// パーティクル最大数
	int maxParticles_ = 1000;
	// エミッタータイプ作成用
	std::string nowEmitterTypeName_ = "Sphere";
};

