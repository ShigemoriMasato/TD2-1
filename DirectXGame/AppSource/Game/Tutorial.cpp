#include "Tutorial.h"

void Tutorial::Initialize(Camera* camera, TextureManager* textureManager)
{
	textureHandle_ = textureManager->LoadTexture("Assets/Texture/tutorial.png");
	resource_ = std::make_unique<DrawResource>();
	resource_->Initialize(ShapeType::Plane);
	resource_->camera_ = camera;
	resource_->position_ = { 6.0f,4.0f, 1.0f };
	resource_->scale_ = { 8, 8, 1.0f };
	resource_->color_ = 0xffffffff;
	resource_->SetTextureHandle(textureHandle_);

}

void Tutorial::Draw(Render* render)
{
	render->Draw(resource_.get());
}
