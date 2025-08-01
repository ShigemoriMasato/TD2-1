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

private:

	std::shared_ptr<Camera> camera_;

	std::shared_ptr<Transform> transform_;

	std::vector<Vector3> catmullPoints_;
	std::vector<Vector3> controllPoints_;
	float t = 0.0f;		//catmullの補完用t
	int lapTimer_ = 0;
	const int lapTime_ = 500;	//1周の時間
	Vector3 targetPos_;
};
