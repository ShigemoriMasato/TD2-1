#include "GridMaker.h"
#include <cmath>

GridLine::GridLine(Camera* camera, LineType type, Vector3 center) : Object(camera, ShapeType::Line) {

	color_ = 0x502000ff;

	center_ = center;
	float thickness = 0.0f;

	if (type == LineType::kHorizontal) {
		
		Vector3 start = { center.x, center.y, center.z + GRID_LINE_LENGTH / 2 };
		Vector3 end = { center.x, center.y, center.z - GRID_LINE_LENGTH / 2 };

		if (static_cast<int>(center.x) % 10 == 0) {

			SetShapeType(ShapeType::ThickLine);
			thickness = 0.3f;

			if (static_cast<int>(center.x) == 0) {
				color_ = 0x500000ff;
				thickness = 0.3f;
			}
		}

		SetLocalPosition(start, end, { thickness });

	} else {

		Vector3 start = { center.x + GRID_LINE_LENGTH / 2, center.y, center.z };
		Vector3 end = { center.x - GRID_LINE_LENGTH / 2, center.y, center.z };
		SetLocalPosition(start, end);

		if (static_cast<int>(center.z) % 10 == 0) {

			SetShapeType(ShapeType::ThickLine);
			thickness = 0.3f;

			if (static_cast<int>(center.z) == 0) {
				color_ = 0x500000ff;
				thickness = 0.3f;
			}
		}

		SetLocalPosition(start, end, { thickness });
	}

	center_ = center;

	type_ = type;
}

void GridLine::AdjustCenter(float center) {
	if (type_ == LineType::kHorizontal) {

		Vector3 start = { center_.x, center_.y, center + GRID_LINE_LENGTH / 2 };
		Vector3 end = { center_.x, center_.y, center - GRID_LINE_LENGTH / 2 };
		SetLocalPosition(start, end);

	} else {

		Vector3 start = { center + GRID_LINE_LENGTH / 2, center_.y, center_.z };
		Vector3 end = { center - GRID_LINE_LENGTH / 2, center_.y, center_.z };
		SetLocalPosition(start, end);

	}
}

GridMaker::GridMaker(Camera* camera, bool isDebugCamera) {
	if (isDebugCamera) {
		debugCamera_ = dynamic_cast<DebugCamera*>(camera);
		camera_ = debugCamera_;
	} else {
		camera_ = camera;
	}

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
	Vector3 mid{};
	if(debugCamera_) {
		mid = debugCamera_->GetCenter();
	} else {
		mid = camera_->GetPosition();
	}
	
	bool isAdjust = false;

	// 縦線の調整
	while (!isAdjust) {
		// グリッド一個分ずれているか
		if(std::fabsf(nowMid.x - mid.x) >= gridSize_.x) {

			// グリッドのずれてる方向を確認
			if (nowMid.x - mid.x < 0) {

				// 右にずれているので、左にある線を削除して、右に追加する
				lines_[0].erase(lines_[0].begin());

				Vector3 center = nowMid;
				center.x = center.x + (kGridCount * gridSize_.x) / 2;
				lines_[0].push_back(std::make_unique<GridLine>(camera_, GridLine::kHorizontal , center));

				// ずらした分だけnowMidを更新
				nowMid.x += gridSize_.x;

			} else {

				// 左にずれているので、右にある線を削除して、左に追加する
				lines_[0].pop_back();

				Vector3 center = nowMid;
				center.x = center.x - (kGridCount * gridSize_.x) / 2;
				lines_[0].insert(lines_[0].begin(), std::make_unique<GridLine>(camera_, GridLine::kHorizontal, center));

				// ずらした分だけnowMidを更新
				nowMid.x -= gridSize_.x;
			}

		} else {
			isAdjust = true;
		}
	}

	isAdjust = false;

	// 横線の調整
	while (!isAdjust) {
		// グリッド一個分ずれているか
		if(std::fabsf(nowMid.z - mid.z) >= gridSize_.y) {

			// グリッドのずれてる方向を確認
			if (nowMid.z - mid.z < 0) {

				// 上にずれているので、下にある線を削除して、上に追加する
				lines_[1].erase(lines_[1].begin());

				Vector3 center = nowMid;
				center.z = center.z + (kGridCount * gridSize_.y) / 2;
				lines_[1].push_back(std::make_unique<GridLine>(camera_, GridLine::kVertical, center));

				// ずらした分だけnowMidを更新
				nowMid.z += gridSize_.y;
			} else {

				// 下にずれているので、上にある線を削除して、下に追加する
				lines_[1].pop_back();

				Vector3 center = nowMid;
				center.z = center.z - (kGridCount * gridSize_.y) / 2;
				lines_[1].insert(lines_[1].begin(), std::make_unique<GridLine>(camera_, GridLine::kVertical, center));

				// ずらした分だけnowMidを更新
				nowMid.z -= gridSize_.y;
			}

		} else {
			isAdjust = true;
		}

	}

	for (int i = 0; i < kGridCount; ++i) {
		lines_[0][i]->AdjustCenter(mid.z);
		lines_[1][i]->AdjustCenter(mid.x);
	}
}

void GridMaker::Draw() const {
	for (int i = 0; i < 2; ++i) {
		for (const auto& line : lines_[i]) {
			line->Draw();
		}
	}
}
