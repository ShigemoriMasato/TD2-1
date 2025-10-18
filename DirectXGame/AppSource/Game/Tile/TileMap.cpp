#include "TileMap.h"
#include "../Physics/PhysicsEngine.h"
#include <json.hpp>

using Json = nlohmann::json;

TileMap::TileMap(PhysicsEngine* physicsEngine)
	: physicsEngine_(physicsEngine)
{
	mpResource_ = std::make_unique<MPResource>();
	physicsEngine_->RegisterTileMap(this);
}

TileMap::~TileMap()
{
	physicsEngine_->UnregisterTileMap(this);
}

void TileMap::LoadMap(TileInfo&& tiles)
{
	tiles_ = std::move(tiles);
	mapSize_.x = static_cast<float>(tiles_.width);
	mapSize_.y = static_cast<float>(tiles_.height);
	physicsEngine_->SetWorldBounds(AABB({0,0,0},Vector3(WorldSize())));

}

TileType TileMap::GetTileInfoAt(int x, int y) const
{
	if (x < 0 || x >= mapSize_.x || y < 0 || y >= mapSize_.y)
	{
		return TileType::Empty;
	}
	size_t index = static_cast<size_t>(x + y * mapSize_.x);

	if (index < tiles_.type.size())
	{
		return tiles_.type[index];
	}
	return TileType::Empty;
}

TileType TileMap::GetTileTypeAt(int x, int y) const
{
	return  GetTileInfoAt(x, y);
}

TileType TileMap::GetTileAtWorldPos(const Vector2& worldPos) const
{
	int x = static_cast<int>(std::floor(worldPos.x / size_.x));
	int y = static_cast<int>(std::floor(worldPos.y / size_.y));

	return GetTileTypeAt(x, y);
}

Vector2 TileMap::WorldSize() const
{
	Vector2 worldSize;
	worldSize.x = size_.x * mapSize_.x;
	worldSize.y = size_.y * mapSize_.y;
	return worldSize;
}

void TileMap::SetModelData(TextureManager* textureManager, ModelData* modelData, Camera* camera)
{
	int index = 0;
	srand(1);
	mpResource_->Initialize(modelData, tiles_.width * tiles_.height);
	MPResource* resource = mpResource_.get();
	for (int y = 0; y < tiles_.height; ++y)
	{
		for (int x = 0; x < tiles_.width; ++x)
		{
			if (GetTileInfoAt(x, y) != TileType::Empty)
			{
				resource->camera_ = camera;
				resource->psoConfig_.isSwapChain = true;
				resource->position_[index] = Vector3(x * size_.x, (mapSize_.y - 1 - y) * size_.y, 0.0f);
				resource->color_[index] = 0x808080ff | (rand() % (x + 1) + 101);
				index++;
			}
		}
	}

	//画像のリソースの先頭をセット
	//int handle = textureManager->LoadTexture("Assets/Texture/Mapchip/Block01.png");
	//textureManager->LoadTexture("Assets/Texture/Mapchip/Block02.png");
	//textureManager->LoadTexture("Assets/Texture/Mapchip/Block03.png");
	//textureManager->LoadTexture("Assets/Texture/Mapchip/Block04.png");
	//その他読み込み
	//
	//mpResource_->textureStartIndex_ = handle;

	//今はとりあえずwhite1x1とかを入れとく
	mpResource_->textureStartIndex_ = 0;
}

void TileMap::Draw(Render* render)
{
	if (size_.x <= 0 || size_.y <= 0)
	{
		return;
	}

	/* for (size_t y = 0; y < mapSize_.y; ++y){
		 for (size_t x = 0; x < mapSize_.x; ++x){
			 size_t index = x + y * size_t(mapSize_.x);

			 if (index < tiles_.type.size() && tiles_.type[index] != TileType::Empty)
			 {
				 const auto& tileInfo = tiles_.type[index];

				 tileInfo.model->position_ = Vector3(x * size_.x, y * size_.y, 0.0f);

				 render->Draw(tileInfo.model.get());
			 }

		 }
	 }*/
	render->Draw(mpResource_.get());
}