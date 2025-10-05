#include "PostEffectResource.h"

PostEffectResource::PostEffectResource() {
}

void PostEffectResource::Initialize() {
	auto windowSize = dxDevice_->GetWindowSize();

	vertexNum_ = 3;
}
