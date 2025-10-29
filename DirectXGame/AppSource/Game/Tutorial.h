#pragma once
#include <Render/Resource/DrawResource.h>
#include <Camera/Camera.h>
#include <Resource/Texture/TextureManager.h>
#include <Render/Render.h>

class Tutorial
{
public:

	void Initialize(Camera* camera, TextureManager* textureManager);

    void Draw(Render* render);
	
	void SetPosition(float x, float y) { if (resource_)resource_->position_ = { x,y,1.0f };}
private:
	int textureHandle_ = 0;

	std::unique_ptr<DrawResource> resource_ = nullptr;

};

