#include "MoveUpLine.h"
#include <ctime>
#include <cstdlib>
#include <algorithm>

MoveUpLine::MoveUpLine(Vector3 start, Vector3 end, int lifeTime, Camera* camera) : Action("MoveLine", nullptr), lag_(lifeTime / 4) {
	srand(static_cast<unsigned int>(time(nullptr)));
	line_ = std::make_unique<MoveUpLineObject>(camera, ShapeType::ThickLine);
	line_->SetLocalPosition(start, start);
	maxLifeTime_ = lifeTime;
}

void MoveUpLine::Execute() {
	++lifeTime_;

	float t = static_cast<float>(lifeTime_) / static_cast<float>(maxLifeTime_ - lag_);
	Vector3 start = MyMath::lerp(start_, end_, t);

	t = static_cast<float>(lifeTime_ - lag_) / static_cast<float>(maxLifeTime_ - lag_);
	t = std::clamp(t, 0.0f, 1.0f);
	Vector3 end = MyMath::lerp(start_, end_, t);

	line_->SetLocalPosition(start, end);
}

void MoveUpLine::ExecuteDraw() {
	line_->Draw();
}
