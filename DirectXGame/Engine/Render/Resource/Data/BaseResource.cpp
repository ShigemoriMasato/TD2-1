#include "BaseResource.h"
#include <Math/MyMath.h>

DXDevice* BaseResource::dxDevice_ = nullptr;
SRVManager* BaseResource::srvManager_ = nullptr;

using namespace Matrix;

Matrix4x4 BaseResource::GetWorldMatrix(const Vector3 scale, const Vector3 rotation, const Vector3 position) {
	if (isSetMatrix_) {
		isSetMatrix_ = false;
		return worldMatrix_;
	}

	return MakeScaleMatrix(scale) *
		MakeRotationMatrix(rotation) *
		MakeTranslationMatrix(position);
}
