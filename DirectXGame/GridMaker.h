#pragma once
#include "Object/Object.h"
#include <memory>
#include <vector>
#include <array>

class GridLine : public Object {
public:

	enum LineType {
		kHorizontal,
		kVertical
	};

	GridLine(Camera* camera, LineType type, Vector3 center);
	~GridLine() = default;

	void Initialize() override {};
	void Update() override {};

private:

};

class GridMaker {
public:

	GridMaker(Camera* camera);
	~GridMaker() = default;

	void Initialize();
	
	void Update();

	void Draw() const;

private:

	std::array<std::vector<std::unique_ptr<GridLine>>, 2> lines_{};
	const Vector2 gridSize_{ 1.0f, 1.0f }; //Gridで囲う空間のサイズ
	Camera* camera_;

	Vector3 nowMid = { 0.0f, 0.0f, 0.0f };

	static const int kGridCount = 50;		//Gridが生成される数

};

