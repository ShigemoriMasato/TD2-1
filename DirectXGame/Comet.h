#pragma once
#include "Object/Object.h"
#include "Scene/Common/CommonData.h"

struct CommetConfig {
	Vector3 position;
};

class Comet : public Object {
public:

	Comet(Camera* camera, CommonData* commonData, Vector3* playerTransformPtr);
	~Comet() = default;

	void Initialize() override;

	void Update() override;

	void SetPosition(const Vector3& position);
	void AddMovement(const Vector3& movement);

	void OnCollision(Object* other) override;

private:

	std::array<int, 3> modelHandle_;

	Vector3* playerPosition_;

};

