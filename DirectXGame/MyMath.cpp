#include "MyMath.h"
#include <cmath>

Matrix4x4 MakeTransformMatrix(const Transform& transform) {
	return MakeScaleMatrix(transform.scale) *
		MakeRotateMatrix(transform.rotation) *
		MakeTranslateMatrix(transform.position);
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {

	return {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		translate.x, translate.y, translate.z, 1
	};

}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	return {
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1
	};

}

Matrix4x4 MakeRotateMatrix(const Vector3& rotation) {
	Matrix4x4 matX = MakeRotetaXMatrix(rotation.x);
	Matrix4x4 matY = MakeRotateYMatrix(rotation.y);
	Matrix4x4 matZ = MakeRotateZMatrix(rotation.z);
	return matX * matY * matZ;
}

Matrix4x4 MakeRotetaXMatrix(float radius) {

	return {
		1, 0, 0, 0,
		0, std::cosf(radius), std::sinf(radius), 0,
		0, -std::sinf(radius), std::cosf(radius), 0,
		0, 0, 0, 1
	};

}

Matrix4x4 MakeRotateYMatrix(float radius) {

	return {
		std::cosf(radius), 0, -std::sinf(radius), 0,
		0, 1, 0, 0,
		std::sinf(radius), 0, std::cosf(radius), 0,
		0, 0, 0, 1
	};

}

Matrix4x4 MakeRotateZMatrix(float radius) {

	return { std::cosf(radius), std::sinf(radius), 0, 0,
		-std::sinf(radius), std::cosf(radius), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

}
