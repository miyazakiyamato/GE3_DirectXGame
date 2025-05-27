#pragma once
#include "BasePipeline.h"

class PipelineFactory{
public:
	static BasePipeline* ChangePipeline(const std::string& pipelineName);
};

