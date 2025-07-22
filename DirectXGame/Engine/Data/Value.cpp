#include "Value.h"
#include <cassert>

std::shared_ptr<Value> MakeValue(std::string argName, std::vector<float> values) {
	Vector2 v2;
	Vector3 v3;
	Vector4 v4;

	switch (values.size()) {
	case 2:
		v2 = { values[0], values[1] };
		return std::make_unique<Vec2Value>(argName, v2);
	case 3:
		v3 = { values[0], values[1], values[2] };
		return std::make_unique<Vec3Value>(argName, v3);
	case 4:
		v4 = { values[0], values[1], values[2], values[3] };
		return std::make_unique<Vec4Value>(argName, v4);
	default:
		assert(false && "MakeValue: format Error");
		return nullptr;
	}

	return nullptr;
}
