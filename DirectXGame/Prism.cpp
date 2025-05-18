#include "Prism.h"

Prism::Prism() {
	const float pie = 3.14159265358f;

	//上段
	lineVertexlist_[0] = { 0.0f, 2.0f, 0.0f, 1.0f };
	//中段
	for (int i = 1; i < 4; ++i) {
		lineVertexlist_[i] = { cosf(pie * 2.0f * (i - 1) / 3.0f) / 2.0f, 0.0f, sinf(pie * 2.0f * (i - 1) / 3.0f) / 2.0f, 1.0f };
	}
	//下段
	lineVertexlist_[4] = { 0.0f, -2.0f, 0.0f, 1.0f };

	circle_ = new Circle();
}

Prism::~Prism() {
	delete circle_;
}

void Prism::Initialize(Matrix4x4* vpMatrix, DirectionalLightData* dLightData, Vector4 color, float radius, int prismNum) {
	const float pie = 3.14159265358f;
	
	//ごり押し場合分け
	if (radius == 4.0f) {
		circle_->Initialize(vpMatrix, dLightData, 64);
	} else {
		circle_->Initialize(vpMatrix, dLightData, 32);
	}
	circle_->SetMaterial({ 0.5f, 0.5f, 0.5f, 1.0f, 1 });

	Transform circleTransform = { 1.0f, 1.0f, 1.0f };
	circle_->Update(circleTransform, radius, 0.01f);

	//初期化
	prismNum_ = prismNum;

	for (int i = 0; i < prismNum_; ++i) {
		Transform transform;
		float theta = pie * 2.0f * float(i) / float(prismNum_);
		transform.position = { cosf(theta) * radius, 0.0f, sinf(theta) * radius };
		transform.rotation = { pie * 2.0f - theta, 0.0f, pie / 2.0f};
		transform.scale = { radius / 16.0f, radius / 4.0f, radius / 16.0f };
		transforms_.push_back(transform);
		positionAngle_.push_back(theta);
	}

	radius_ = radius;

	material_.color = color;
	material_.enableLighting = false;
	vpMatrix_ = vpMatrix;
	dLightData_ = dLightData;
}

void Prism::Update(Kind kind) {
	int direction;
	if (kind == kround) {
		direction = 1;
	} else {
		direction = 0;
	}

	const float pie = 3.14159265358f;
	for (int i = 0; i < prismNum_; ++i) {
		positionAngle_[i] += float(int(direction) * 2 - 1) / 100;
		transforms_[i].position.x = cosf(positionAngle_[i]) * radius_;
		transforms_[i].position.z = sinf(positionAngle_[i]) * radius_;

		if (kind != kouter) {
			transforms_[i].rotation.x = pie * 2.0f - positionAngle_[i];
			transforms_[i].rotation.y += 0.02f;
			transforms_[i].rotation.z -= 0.02f;
		} else {
			transforms_[i].rotation.x = 0.0f;
			transforms_[i].rotation.y += 0.04f;
			transforms_[i].rotation.z = 0.0f;
		}

		if (positionAngle_[i] > pie) {
			positionAngle_[i] -= pie * 2.0f;
		}

		if (transforms_[i].rotation.x > pie) {
			transforms_[i].rotation.x -= pie * 2.0f;
		}

		if (transforms_[i].rotation.y > pie) {
			transforms_[i].rotation.y -= pie * 2.0f;
		}

		if (transforms_[i].rotation.z < -pie) {
			transforms_[i].rotation.z += pie * 2.0f;
		}
	}

	switch (kind) {
	case Prism::kinner:
		ImGui::Begin("Prism");
		break;
	case Prism::kround:
		ImGui::Begin("Prism1");
		break;
	case Prism::kouter:
		ImGui::Begin("Prism2");
		break;
	}

	ImGui::SliderFloat3("Position", &transforms_[0].position.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("Rotation", &transforms_[0].rotation.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("Scale", &transforms_[0].scale.x, 0.0f, 10.0f);
	ImGui::ColorEdit4("Color", &material_.color.x);
	const char* items[] = { "UVChecker", "white1x1" };
	int selectTexture = textureHandle_;
	if (ImGui::Combo("Dropdown", &selectTexture, items, IM_ARRAYSIZE(items))) {
		textureHandle_ = selectTexture;
	}
	ImGui::End();
}

void Prism::Draw(MyDirectX* md) {
	for (int i = 0; i < prismNum_; ++i) {
		md->DrawPrism(
			MakeTransformMatrix(transforms_[i]),
			MakeTransformMatrix(transforms_[i])  * (*vpMatrix_),
			material_,
			*dLightData_,
			textureHandle_);
	}

	circle_->Draw(md);
}
