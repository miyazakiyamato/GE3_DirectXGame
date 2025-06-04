#include "PipelineFactory.h"
#include "ModelPipeline.h"
#include "SkinningObject3dPipeline.h"
#include "SpritePipeline.h"
#include "ParticlePipeline.h"
#include "Line3DPipeline.h"
#include "OffScreenPipeline.h"

BasePipeline* PipelineFactory::ChangePipeline(const std::string& pipelineName) {
	if (pipelineName == "Object3d") {
		return new ModelPipeline();
	} else if (pipelineName == "SkinningObject3d") {
		return new SkinningObject3dPipeline();
	} else if (pipelineName == "Sprite") {
		return new SpritePipeline();
	} else if (pipelineName == "Particle") {
		return new ParticlePipeline();
	} else if (pipelineName == "Line3D") {
		return new Line3DPipeline();
	} else if (pipelineName == "OffScreen/Fullscreen"||
		pipelineName == "OffScreen/Grayscale" ||
		pipelineName == "OffScreen/Vignette" ||
		pipelineName == "OffScreen/BoxFilter" ||
		pipelineName == "OffScreen/GaussianFilter") {
		return new OffScreenPipeline();
	}
	assert(0);
	return nullptr;
}
