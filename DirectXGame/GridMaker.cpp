#include "GridMaker.h"
#include <cmath>

GridLine::GridLine(Camera* camera, LineType type, Vector3 center) : Object(camera,
	ShapeType::Line) {

	if (type == LineType::kHorizontal) {
		
		Vector3 start = { center.x, center.y, center.z + 25.0f };
		Vector3 end = { center.x, center.y, center.z - 25.0f };
		SetLocalPosition(start, end);

	} else {

		Vector3 start = { center.x + 25.0f, center.y, center.z };
		Vector3 end = { center.x - 25.0f, center.y, center.z };
		SetLocalPosition(start, end);

	}
}

GridMaker::GridMaker(Camera* camera) {
	camera_ = camera;

	Initialize();
}

void GridMaker::Initialize() {
	Transform transform{};

	for (int i = 0; i < 2; ++i) {

		lines_[i].clear(); // 既存のラインをクリア

		for (int j = 0; j < kGridCount; ++j) {
			// i == 0: 縦線, i == 1: 横線
			Vector3 center = nowMid;
			if (i == 0) {
				center.x = center.x - (kGridCount * gridSize_.x) / 2 + float(j * gridSize_.x);
			} else {
				center.z = center.z - (kGridCount * gridSize_.y) / 2 + float(j * gridSize_.y);
			}

			lines_[i].push_back(std::make_unique<GridLine>(camera_, i == 0 ? GridLine::kHorizontal : GridLine::kVertical, center));

		}
	}
}

void GridMaker::Update() {
	Vector3 mid = camera_->GetPosition();
	bool fixed = false;

	while (!fixed) {
		if (fabsf(nowMid.x) + 1 < fabsf(mid.x)) {
			if (nowMid.x > 0) {
				++nowMid.x;

				lines_[0].erase(lines_[0].begin());

				Vector3 center = nowMid;
				center.x = nowMid.x - (kGridCount * gridSize_.x) / 2 - (kGridCount - 1) * gridSize_.x;

				lines_[0].push_back(std::make_unique<GridLine>(camera_, GridLine::kVertical, center));

			} else {
				--nowMid.x;

				lines_[0].erase(lines_[0].begin());

				Vector3 center = nowMid;
				center.x = nowMid.x - (kGridCount * gridSize_.x) / 2;

				lines_[0].push_back(std::make_unique<GridLine>(camera_, GridLine::kVertical, center));
			}
		} else {
			fixed = true;
		}
	}

	fixed = false;

	while (!fixed) {
		if (fabsf(nowMid.z) + 1 < fabsf(mid.z)) {
			if (nowMid.z > 0) {
				++nowMid.z;

				lines_[1].erase(lines_[1].begin());

				Vector3 center = nowMid;
				center.z = nowMid.z - (kGridCount * gridSize_.y) / 2 - (kGridCount - 1) * gridSize_.y;

				lines_[1].push_back(std::make_unique<GridLine>(camera_, GridLine::kVertical, center));

			} else {
				--nowMid.z;

				lines_[1].erase(lines_[1].begin());

				Vector3 center = nowMid;
				center.z = nowMid.z - (kGridCount * gridSize_.y) / 2;

				lines_[1].push_back(std::make_unique<GridLine>(camera_, GridLine::kVertical, center));
			}
		} else {
			fixed = true;
		}
	}
}

void GridMaker::Draw() const {
	for (int i = 0; i < 2; ++i) {
		for (const auto& line : lines_[i]) {
			line->Draw();
		}
	}
}
