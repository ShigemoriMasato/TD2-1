#include "BaseResource.h"
#include <Math/MyMath.h>

DXDevice* BaseResource::dxDevice_ = nullptr;
SRVManager* BaseResource::srvManager_ = nullptr;
TextureManager* BaseResource::textureManager_ = nullptr;

using namespace Matrix;

void BaseResource::StaticInitialize(DXDevice* device, SRVManager* srvManager, TextureManager* textureManager) {
	dxDevice_ = device;
	srvManager_ = srvManager;
	textureManager_ = textureManager;
}

Matrix4x4 BaseResource::GetWorldMatrix(const Vector3 scale, const Vector3 rotation, const Vector3 position) {
	if (isSetMatrix_) {
		isSetMatrix_ = false;
		return worldMatrix_;
	}

	return MakeScaleMatrix(scale) *
		MakeRotationMatrix(rotation) *
		MakeTranslationMatrix(position);
}
