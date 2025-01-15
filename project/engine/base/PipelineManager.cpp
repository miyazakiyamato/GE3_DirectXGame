#include "PipelineManager.h"
#include "ModelPipeline.h"
#include "Logger.h"

using namespace Microsoft::WRL;
using namespace Logger;

void PipelineManager::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	graphicsPipelineState_[PipelineState::kModel] = std::make_unique<ModelPipeline>();
	graphicsPipelineState_[PipelineState::kModel].get()->Initialize(dxCommon_);
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