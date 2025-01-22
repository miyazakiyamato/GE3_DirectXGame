#include "PipelineManager.h"
#include "ModelPipeline.h"
#include "SpritePipeline.h"
#include "ParticlePipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

PipelineManager* PipelineManager::instance = nullptr;

PipelineManager* PipelineManager::GetInstance(){
	if (instance == nullptr) {
		instance = new PipelineManager;
	}
	return instance;
}

void PipelineManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	graphicsPipelineState_[PipelineState::kModel] = std::make_unique<ModelPipeline>();
	graphicsPipelineState_[PipelineState::kModel].get()->Initialize(dxCommon_);
	graphicsPipelineState_[PipelineState::kSprite] = std::make_unique<SpritePipeline>();
	graphicsPipelineState_[PipelineState::kSprite].get()->Initialize(dxCommon_);
	graphicsPipelineState_[PipelineState::kParticle] = std::make_unique<ParticlePipeline>();
	graphicsPipelineState_[PipelineState::kParticle].get()->Initialize(dxCommon_);
}

void PipelineManager::Finalize(){
	delete instance;
	instance = nullptr;
}

void PipelineManager::DrawSetting(PipelineState pipelineState, BlendMode blendMode){
	//読み込み済みパイプラインを検索
	assert(graphicsPipelineState_.contains(pipelineState));
	graphicsPipelineState_[pipelineState]->DrawSetting(blendMode);
}

void PipelineManager::CreateGraphicsPipeline(PipelineState pipelineState, BlendMode blendMode) {
	//読み込み済みパイプラインを検索
	assert(graphicsPipelineState_.contains(pipelineState));
	graphicsPipelineState_[pipelineState]->CreateGraphicsPipeline(blendMode);
}