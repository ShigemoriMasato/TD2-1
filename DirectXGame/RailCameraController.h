#pragma once
#include "Engine/Camera/Camera.h"
#include <memory>
#include <vector>

class RailCameraController {
public:

	RailCameraController(Camera* camera = nullptr);
	~RailCameraController() = default;

	void Initialize();
	void Update();
	void Draw(Camera* camera);

	Camera GetCamera() { return *camera_; }
	Camera* GetCameraPtr() { return camera_.get(); }
	float* GetSpeedPtr() { return &effectSpeed_; }

private:

	std::shared_ptr<Camera> camera_;

	std::shared_ptr<Transform> transform_;

	std::vector<Vector3> catmullPoints_;
	std::vector<DistanceSample> distanceSamples_;
	std::vector<Vector3> controllPoints_;

	float speed_ = 0.5f;
	float effectSpeed_ = 0.0f;
	float moveDistance_ = 0.0f;
	const float attenuation_ = 0.99f;

	Vector3 targetPos_;
};
