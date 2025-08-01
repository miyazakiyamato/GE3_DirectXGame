#pragma once
#include <string>
#include <map>
#include "DirectXCommon.h"

class TimeManager{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	struct Timer{
		float kLimitTime = 0.5f;
		float timeCount = 0.0f;
		bool isStart = false;
	};
	struct PerFrame {
		float time; // 時間
		float deltaTime; // 1フレームの経過時間
	};
public:
	//シングルインスタンスの取得
	static TimeManager* GetInstance();
	//終了
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void TimeSpeedReset();

	void SetDeltaTimeSpeedStart(float timeSpeed,float kLimitTime);
	//デルタタイム
	static inline float deltaTime_ = 1.0f / 60.0f;
	static inline const float kFlamTime_ = 1.0f / 60.0f;
private://シングルインスタンス
	static TimeManager* instance;

	TimeManager() = default;
	~TimeManager() = default;
	TimeManager(TimeManager&) = delete;
	TimeManager& operator=(TimeManager&) = delete;
private:
	DirectXCommon* dxCommon_ = nullptr; // DirectXの共通インスタンス

	float timeSpeed_ = kFlamTime_;
	float kLimitTime_ = 0.5f;
	float timeCount_ = 0.0f;
	std::map<std::string, Timer> timers_;

	//timeのリソース
	ComPtr<ID3D12Resource> perFrameResource_;
	PerFrame* perFrameData_ = nullptr; // フレームごとの時間情報
public:
	Timer GetTimer(std::string name) { return timers_[name]; }
	// 現在の時間を取得
	float GetTimeCurrent() const { return perFrameData_->time; }
	ID3D12Resource* GetPerFrameResource() const { return perFrameResource_.Get(); }

	void SetTimer(std::string name, float kLimitTime);
};

