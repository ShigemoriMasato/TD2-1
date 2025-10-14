#pragma once
#include "../BaseObject.h"
#include <Math/MyMath.h>


class BaseEnemy : public BaseObject
{
public:

	virtual ~BaseEnemy() = default;
	virtual void Initialize(ModelData* modelData, Camera* camera) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(Render* render) = 0;

	// プレイヤーの位置を設定（敵がプレイヤーの位置を知るため）
	virtual void SetPlayerPosition(const Vector3& playerPos) { playerPosition_ = playerPos; }

	// 敵がアクティブかどうか
	bool IsActive() const { return isActive_; }
	void SetActive(bool active) { isActive_ = active; }

protected:
	Vector3 playerPosition_ = { 0.0f, 0.0f, 0.0f };
	bool isActive_ = true;


};

