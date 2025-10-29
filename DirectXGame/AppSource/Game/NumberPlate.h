#pragma once
#include <Render/Render.h>	

class NumberPlate {
public:

	NumberPlate() = default;
	~NumberPlate() = default;

	void Initialize(TextureManager* textureManager, int digit, bool isTimer);
	void Update(int num);
	void Draw(Render* render);

	void SetPosition(Vector3 position);
	void SetScale(Vector3 scale) { transform_.scale = scale; }

private:

	std::unique_ptr<Camera> camera_ = nullptr;
	std::vector<std::unique_ptr<DrawResource>> res_;
	float rotationAngle_ = 0.0f;
	std::vector<int> textureHandles_;

	Transform transform_{};

	int digit_ = 0;
	bool isTimer_ = false;

	float time_ = 0.0f;

};
