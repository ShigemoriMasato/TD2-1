#include "Center.h"

void Center::Initialize(Matrix4x4* vpMatrix, DirectionalLightData* dLightData) {
	vpMatrix_ = vpMatrix;
	dLightData_ = dLightData;

	largeBox_ = {};
	largeBox_.position = { 0.0f, 0.0f, 0.0f };
	largeBox_.rotation = { 0.0f, 0.0f, 0.0f };
	largeBox_.scale = { 1.0f, 1.0f, 1.0f };

	smallBox_[0] = {};

	smallBox_[1] = {};

	sphere_ = {};

	largeMaterial_.color = { 0.3f, 0.05f, 0.10f, 1.0f };

	smallMaterial_.color = { 0.23f, 0.06f, 0.175f, 1.0f };

	sphereMaterial_.color = { 0.7f, 0.7f, 0.65f, 1.0f };
}

void Center::Update() {
	const int cycle = 120;

	++frame_;

	largeBox_.rotation.y -= 0.03f;

	if (frame_ > cycle * 2) {
		frame_ -= cycle * 2;
	}

	if (frame_ % (cycle * 2) < cycle) {
		float theta = frame_ * 3.14f / cycle;
		largeBoxScaleRate_ = sinf(theta);
		largeBox_.scale.y = (1.0f - largeBoxScaleRate_) * 0.7f;
		largeBox_.scale.x = (largeBoxScaleRate_) * 1.3f;
		largeBox_.scale.z = (largeBoxScaleRate_) * 1.3f;
	} else {
		largeBox_.scale = {};
	}

	for (int i = 0; i < 2; ++i) {
		if (frame_ % (cycle * 2) > cycle) {
			smallBoxScaleRate_ = sinf((frame_ - cycle) * 3.14f / float(cycle));
			smallBox_[i].scale.y = 0.7f;
			smallBox_[i].scale.x = (smallBoxScaleRate_) * 0.7f + 0.01f;
			smallBox_[i].scale.z = (smallBoxScaleRate_) * 0.7f + 0.01f;

			smallBox_[i].position.y = (i * 2 - 1) * smallBoxScaleRate_ * 1.4f;

			smallBox_[i].rotation.y -= 0.03f;

			if (smallBox_[i].rotation.y < -3.14f) {
				smallBox_[i].rotation.y += 3.14f * 2.0f;
			}
		} else {
			smallBox_[i].scale = {};
		}
	}

	if (frame_ % (cycle * 2) > cycle) {
		sphere_.scale = Vector3{ 1.0f, 1.0f, 1.0f } * smallBoxScaleRate_;
	} else {
		sphere_.scale = {};
	}

	ImGui::Begin("Center");
	ImGui::ColorEdit4("LargeBoxColor", &largeMaterial_.color.x);
	ImGui::ColorEdit4("SmallBoxColor", &smallMaterial_.color.x);
	ImGui::ColorEdit4("SphereColor", &sphereMaterial_.color.x);
	ImGui::End();

}

void Center::Draw(MyDirectX* md) {

	md->DrawPrism(
		MakeTransformMatrix(largeBox_),
		MakeTransformMatrix(largeBox_)  * (*vpMatrix_),
		largeMaterial_,
		*dLightData_,
		1
	);

	for (int i = 0; i < 2; ++i) {
		md->DrawPrism(
			MakeTransformMatrix(smallBox_[i]),
			MakeTransformMatrix(smallBox_[i]) * (*vpMatrix_),
			smallMaterial_,
			*dLightData_,
			1
		);
	}

	md->DrawSphere(
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		MakeTransformMatrix(sphere_),
		MakeTransformMatrix(sphere_) * (*vpMatrix_),
		sphereMaterial_,
		*dLightData_,
		1
	);
}
