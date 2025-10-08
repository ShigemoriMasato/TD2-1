#pragma once
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>

#include <memory>

class Player {
public:

	Player();
	~Player();

	void Initialize(ModelData* modelData);
	void Update();
	void Draw(Render* render);



private:

	std::unique_ptr<ModelResource> modelResource_;

	const float jumpSpeed_ = 1.0f;
	const float gravity_ = 9.8f;

	const float xSpeed_ = 1.0f;

};

