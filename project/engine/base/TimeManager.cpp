#include "TimeManager.h"
#include <cassert>

TimeManager* TimeManager::instance = nullptr;

TimeManager* TimeManager::GetInstance(){
	if (instance == nullptr) {
		instance = new TimeManager;
	}
	return instance;
}

void TimeManager::Finalize(){
	delete instance;
	instance = nullptr;
}

void TimeManager::Initialize(){
	deltaTime_ = kFlamTime_;
}

void TimeManager::Update(){
	if (timeSpeed_ != deltaTime_) {
		deltaTime_ = timeSpeed_;
	}
	timeSpeed_ = deltaTime_;

	if (timeCount_ != 0.0f) {
		timeCount_ += kFlamTime_;
		if (timeCount_ > kLimitTime_) {
			timeCount_ = 0.0f;
			deltaTime_ = kFlamTime_;
		}
	}
	for (auto& [name, timer] : timers_) {
		if (timer.isStart == true) {
			timer.timeCount += deltaTime_;
			if (timer.timeCount > timer.kLimitTime) {
				timer.timeCount = 0.0f;
				timer.isStart = false;
			}
		}
	}

	currentTime_ += deltaTime_; // フレームごとに時間を加算
}

void TimeManager::TimeSpeedReset(){
	deltaTime_ = kFlamTime_;
}

void TimeManager::SetTimeSpeedStart(float timeSpeed, float kLimitTime){
	kLimitTime_ = kLimitTime;
	timeSpeed_ = timeSpeed;
	timeCount_ = kFlamTime_;
}

void TimeManager::SetTimer(std::string name, float kLimitTime){
	Timer& timer = timers_[name];
	timer.isStart = true;
	timer.kLimitTime = kLimitTime;
	timer.timeCount = 0.0f;
}