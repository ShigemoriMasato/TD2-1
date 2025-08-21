#pragma once
#include "Object/Object.h"
#include "Object/Collision.h"

class AccelerateGate : public Object {
public:

	static void SetHandle(int handle);

	AccelerateGate(Camera* camera, Vector3 position, Vector3 rotation = {});

	void Initialize() {};
	void Update() {};

	void SetPosition(Vector3 pos) { transform_->position = pos; }

	void OnCollision(Object* other) override;

	Collision* GetCollision() { return collision_.get(); }

private:

	static int modelHandle_;

	bool hited_ = false;//当たり判定をTriggerにする用

	std::shared_ptr<Collision> collision_ = nullptr;

};

