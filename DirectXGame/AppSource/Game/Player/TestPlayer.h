#pragma once
#include "../BaseObject.h"
#include <Common/KeyConfig/KeyManager.h>


class TestPlayer : public BaseObject
{
public:
	void Initialize(ModelData* modelData, Camera* camera) override;
	void Update(float deltaTime) override;
	void OnCollision(BaseObject* other) override;
	void SetKeyConfig(std::unordered_map<Key, bool>* keyConfig) { key_ = keyConfig; }

private:

	std::unordered_map<Key, bool>* key_ = nullptr;

};

