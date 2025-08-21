#include "Object.h"
#include "../Engine/Render/Render.h"

namespace {
	Vector4 Convert(Vector3 vec) {
		return Vector4(vec.x, vec.y, vec.z, 1.0f);
	}
}

using namespace MyMath;

void Object::Draw(const Matrix4x4* worldMatrix) const {

	if (!camera_) {
		return;
	}

	if (type_ < ShapeType::Triangle || type_ >= ShapeType::Count) {
		return;
	}

	if (handle_ < 0) {
		return;
	}

	Matrix4x4 worldMat;
	if (worldMatrix) {
		worldMat = *worldMatrix;
	} else {
		worldMat = MakeAffineMatrix(*transform_);
	}

	MaterialData materialData;
	materialData.color = {
		float((color_ & 0xff000000) >> 24) / 255.0f,
		float((color_ & 0x00ff0000) >> 16) / 255.0f,
		float((color_ & 0x0000ff00) >> 8) / 255.0f,
		float(color_ & 0x000000ff) / 255.0f
	};

	Render::SetIsWireframe(isWireframe_);

	switch (type_) {
	case ShapeType::Triangle:

		Render::DrawTriangle(Convert(lt), Convert(rt), Convert(lb),
			worldMat, camera_, materialData, {}, handle_
		);

		break;

	case ShapeType::Sphere:

		Render::DrawSphere(lt.Length(), worldMat, camera_, materialData, {}, handle_);

		break;

	case ShapeType::Cube:

		Render::DrawBox(worldMat, camera_, materialData, {}, handle_);

		break;

	case ShapeType::Sprite:

		Render::DrawSprite(
			Convert(lt), Convert(rt), Convert(lb), Convert(rb),
			worldMat, camera_, materialData, {}, handle_
		);

		break;

	case ShapeType::Line:

		Render::DrawLine(
			Convert(lt), Convert(rt),
			worldMat, camera_, materialData, {}, handle_
		);

		break;

	case ShapeType::ThickLine:

		Vector3 dir = (lt - rt).Normalize();										// 線の方向
		Vector3 mid = (lt + rt) * 0.5f;												// 中心点
		Vector3 view = (camera_->GetPosition() - mid).Normalize();					// カメラ方向
		Vector3 normal = cross(dir, view).Normalize();								// 線とカメラに垂直な方向
		Vector3 offset = normal * (lb.Length() / 2.0f);								// 太さ分のオフセット

		Render::DrawSprite(Convert(lt + offset), Convert(lt - offset), Convert(rt + offset), Convert(rt - offset),
			worldMat, camera_, materialData, {}, handle_
		);

		break;

	case ShapeType::Model:

		Render::DrawModel(handle_, worldMat, camera_, materialData, {});

		break;
	}
}

void Object::SetLocalPosition(const Vector3& lt, const Vector3& rt, const Vector3& lb, const Vector3& rb) {
	if (lt != UniqueNumber::Vec3::min) {
		this->lt = lt;
	}
	if (rt != UniqueNumber::Vec3::min) {
		this->rt = rt;
	}
	if (lb != UniqueNumber::Vec3::min) {
		this->lb = lb;
	}
	if (rb != UniqueNumber::Vec3::min) {
		this->rb = rb;
	}
}
