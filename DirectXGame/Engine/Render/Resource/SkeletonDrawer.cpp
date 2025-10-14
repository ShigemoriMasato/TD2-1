#include "SkeletonDrawer.h"

void SkeletonDrawer::Initialize(Skeleton skeleton, Animation anim, Camera* camera) {
	SkeletonUpdate(skeleton);

	for (const auto& joint : skeleton.joints) {
		if (joint.parent.has_value()) {

			auto res = std::make_unique<DrawResource>();
			res->Initialize(2, 0);

			res->localPos_ = {
				{
					joint.skeletonSpaceMatrix.m[3][0],
					joint.skeletonSpaceMatrix.m[3][1],
					joint.skeletonSpaceMatrix.m[3][2],
				},

				{
					skeleton.joints[joint.parent.value()].skeletonSpaceMatrix.m[3][0],
					skeleton.joints[joint.parent.value()].skeletonSpaceMatrix.m[3][1],
					skeleton.joints[joint.parent.value()].skeletonSpaceMatrix.m[3][2],
				}
			};

			res->color_ = 0xff8000ff;
			res->camera_ = camera;
			res->psoConfig_.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			res->psoConfig_.isSwapChain = true;

			resource_.push_back(std::move(res));

			resMap_.emplace_back(std::make_pair(joint.index, skeleton.joints[joint.parent.value()].index));
		}
	}
	skeleton_ = skeleton;
	anim_ = anim;
}

void SkeletonDrawer::Update() {
	animTime_ += 1.f / 60;
	animTime_ = std::fmod(animTime_, anim_.duration);
	SkeletonAnimation(skeleton_, anim_, animTime_);
	SkeletonUpdate(skeleton_);

	for (int i = 0; i < resMap_.size(); ++i) {
		int f = resMap_[i].first;
		int s = resMap_[i].second;

		auto& res = resource_[i];

		res->localPos_ = {
			{
				skeleton_.joints[f].skeletonSpaceMatrix.m[3][0],
				skeleton_.joints[f].skeletonSpaceMatrix.m[3][1],
				skeleton_.joints[f].skeletonSpaceMatrix.m[3][2]
			},
			{
				skeleton_.joints[s].skeletonSpaceMatrix.m[3][0],
				skeleton_.joints[s].skeletonSpaceMatrix.m[3][1],
				skeleton_.joints[s].skeletonSpaceMatrix.m[3][2]
			}
		};
	}
}

void SkeletonDrawer::Draw(Render* render) {

	for (const auto& res : resource_) {
		render->Draw(res.get());
	}
}
