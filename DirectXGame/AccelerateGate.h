#pragma once
#include "Object/Object.h"

class AccelerateGate : public Object {
public:

	static void SetHandle(int handle);

	AccelerateGate(Camera* camera, Vector3 position, Vector3 rotation = {});

	void Initialize() override {};
	void Update() override {};

	void SetPosition(Vector3 pos) { transform_->position = pos; }

	void OnCollision(Object* other) override;

private:

	static int modelHandle_;

	bool hited_ = false;//当たり判定をTriggerにする用

};

