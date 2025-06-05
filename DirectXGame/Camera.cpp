#include "Camera.h"

void Camera::SetPerspectiveFovMatrix(PerspectiveFovDesc desc) {
	projectionMatrix = MakePerspectiveFovMatrix(desc.fovY, desc.aspectRatio, desc.nearClip, desc.farClip);
}

void Camera::SetOrthographicMatrix(OrthographicDesc desc) {
	projectionMatrix = MakeOrthographicMatrix(desc.left, desc.top, desc.right, desc.bottom, desc.nearClip, desc.farClip);
}

void Camera::SetViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	viewportMatrix = MakeViewportMatrix(left, top, width, height, minDepth, maxDepth);
}

void Camera::MakeMatrix() {
	vpvMatrix = Inverse(MakeTransformMatrix(transform)) * projectionMatrix * viewportMatrix;
}

Matrix4x4 Camera::GetViewportMatrix() const {
	return vpvMatrix;
}
