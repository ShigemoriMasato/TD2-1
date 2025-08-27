#pragma once
#include "Object/Object.h"
#include "Object/Collision.h"
#include "AccelerateGateConfig.h"
#include "RankBan.h"

class AccelerateGate : public Object {
public:

	static void SetHandle(int handle);

	AccelerateGate(Camera* camera);

	void Initialize() {};
	void Update();
	void Draw(const Matrix4x4* worldMatrix = nullptr) const override;

	void OnCollision(Object* other);

	void SetPosition(Vector3 pos) { transform_->position = pos; }
	void SetConfig(AccelerateGateConfig config);

	Collision* GetCollision() { return collision_.get(); }
	AccelerateGateConfig GetConfig() const;
	Vector3 GetCameraPos();

private:

	static int modelHandle_;

	bool hited_ = false;//当たり判定をTriggerにする用

	std::shared_ptr<RenderCollision> collision_ = nullptr;

};
