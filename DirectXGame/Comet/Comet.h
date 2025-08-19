#pragma once
#include "../Object/Object.h"

struct CometConfig {
	Vector3 position;
};

class Comet : public Object {
public:

	Comet(Camera* camera, std::array<int, 3> modelHandle, Vector3* playerTransformPtr);
	~Comet() = default;

	void Initialize() override;

	void Update() override;

	void SetPosition(const Vector3& position);
	void AddMovement(const Vector3& movement);

	void OnCollision(Object* other) override;

	CometConfig GetConfig() const;

	void SetConfig(const CometConfig& config);

private:

	std::array<int, 3> modelHandle_;

	Vector3* playerPosition_;

};

