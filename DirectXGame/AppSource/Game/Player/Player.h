#pragma once
#include"../BaseObject.h"


class Player : public BaseObject {
public:

	Player();
	~Player();

	void Initialize(ModelData* modelData, Camera* camera)override;
	void Update()override;

private:

};

