#include "Camera.h"
#include "../../externals/imgui/imgui.h"

using namespace Matrix;
using namespace MyMath;

namespace {
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		return {
			cot(fovY / 2) / aspectRatio, 0, 0, 0,
			0, cot(fovY / 2), 0, 0,
			0, 0, farClip / (farClip - nearClip), 1,
			0, 0, (-nearClip * farClip) / (farClip - nearClip), 0
		};
	}

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
		return {
			2 / (right - left), 0, 0, 0,
			0, 2 / (top - bottom), 0, 0,
			0, 0, 1 / (farClip - nearClip), 0,
			(left + right) / (left - right), (top + bottom) / (bottom - top), nearClip / (nearClip - farClip), 1
		};
	}

	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
		return {
			width / 2, 0, 0, 0,
			0, -height / 2, 0, 0,
			0, 0, maxDepth - minDepth, 0,
			left + width / 2, top + height / 2, minDepth, 1
		};
	}
}

void Camera::SetProjectionMatrix(PerspectiveFovDesc desc) {
	projectionMatrix = MakePerspectiveFovMatrix(desc.fovY, desc.aspectRatio, desc.nearClip, desc.farClip);
}

void Camera::SetProjectionMatrix(OrthographicDesc desc) {
	projectionMatrix = MakeOrthographicMatrix(desc.left, desc.top, desc.right, desc.bottom, desc.nearClip, desc.farClip);
}

void Camera::MakeMatrix() {
	vpMatrix = transform_ * projectionMatrix;
}

void Camera::SetTransform(const Transform& transform) {
	this->transform_ = MakeScaleMatrix(transform.scale) * MakeRotationMatrix(transform.rotation) * Inverse(MakeTranslationMatrix(transform.position));
}

void Camera::SetTransform(const Matrix4x4& matrix) {
	transform_ = matrix;
}

Matrix4x4 Camera::VPMatrix() const {
	return vpMatrix;
}

Vector3 Camera::GetPosition() const {
	return Vector3(-transform_.m[3][0], -transform_.m[3][1], -transform_.m[3][2]);
}
