#pragma once
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>

class SkeletonDrawer {
public:

	SkeletonDrawer() = default;
	~SkeletonDrawer() = default;

	void Initialize(Skeleton skeleton, Animation anim, Camera* camera);
	void Update();
	void Draw(Render* render);

private:

	Skeleton skeleton_;
	Animation anim_;
	float animTime_ = 0.0f;

	std::vector<std::unique_ptr<DrawResource>> resource_;
	std::vector<std::pair<int, int>> resMap_;

};

