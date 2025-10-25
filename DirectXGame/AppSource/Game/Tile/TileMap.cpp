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
	physicsEngine_->SetWorldBounds(AABB({ 0,0,0 }, Vector3(WorldSize())));

	MPResource* resource = mpResource_.get();
	resource->InitializeTileTexture(tiles_.width, tiles_.height);
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

Vector3 TileMap::GetWorldPos(int x, int y) const
{
	return Vector3(x * size_.x, mapSize_.y - 1 - y, 0.0f);
}

void TileMap::SetModelData(TextureManager* textureManager, ModelData* modelData, Camera* camera)
{
	mpResource_->Initialize(modelData, tiles_.width * tiles_.height);
	MPResource* resource = mpResource_.get();
	for (int y = 0; y < tiles_.height; ++y)
	{
		for (int x = 0; x < tiles_.width; ++x)
		{
			int index = x + y * tiles_.width;
			if (GetTileInfoAt(x, y) != TileType::Solid)
			{
				continue;
			}
			else
			{
				resource->camera_ = camera;
				resource->position_[index] = GetWorldPos(x, y);
				resource->color_[index] = 0x808080FF;
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

//DDA（Digital Differential Analyzer）
bool TileMap::HasTile(const Vector3& startPos, const Vector3& endPos, TileType type, Vector3* outEndPos, bool includeStartTile) const
{
	Vector2 dir = endPos - startPos;
	float length = dir.Length();
	if (length <= 0.0001f) return false;
	dir /= length;

	const float tileWidth = size_.x;
	const float tileHeight = size_.y;

	int x = static_cast<int>(std::floor(startPos.x / tileWidth));
	int y = static_cast<int>(std::floor(WorldSize().y - 1 - startPos.y / tileHeight));

	int endX = static_cast<int>(std::floor(endPos.x / tileWidth));
	int endY = static_cast<int>(std::floor(WorldSize().y - 1 - endPos.y / tileHeight));

	int stepX = (dir.x > 0.0f) ? 1 : (dir.x < 0.0f ? -1 : 0);
	int stepY = (dir.y > 0.0f) ? 1 : (dir.y < 0.0f ? -1 : 0);

	float tMaxX = (stepX > 0) ? ((x + 1) * tileWidth - startPos.x) / dir.x
		: (stepX < 0) ? (x * tileWidth - startPos.x) / dir.x
		: std::numeric_limits<float>::infinity();

	float tMaxY = (stepY > 0) ? ((y + 1) * tileHeight - startPos.y) / dir.y
		: (stepY < 0) ? (y * tileHeight - startPos.y) / dir.y
		: std::numeric_limits<float>::infinity();

	float tDeltaX = (stepX != 0) ? tileWidth / std::abs(dir.x) : std::numeric_limits<float>::infinity();
	float tDeltaY = (stepY != 0) ? tileHeight / std::abs(dir.y) : std::numeric_limits<float>::infinity();

	const float maxDist = length;
	float traveled = 0.0f;

	bool firstTile = true;
	while (true)
	{
		if (!firstTile || includeStartTile)
		{
			TileType tile = GetTileTypeAt(x, y);
			if (tile == type)
			{
				if (outEndPos) *outEndPos = { (float)x, (float)y, 0.0f };
				return true;
			}
		}
		firstTile = false;

		if (std::abs(x - endX) < 1 && std::abs(y - endY) < 1)
			break;

		if (tMaxX < tMaxY)
		{
			traveled = tMaxX;
			tMaxX += tDeltaX;
			x += stepX;
		}
		else
		{
			traveled = tMaxY;
			tMaxY += tDeltaY;
			y += stepY;
		}

		if (traveled > maxDist)
			break;
	}

	return false;
}

void TileMap::TriggerWaveAtTile(int tileX, int tileY, float radius, float intensity)
{
	if (tileX < 0 || tileX >= tiles_.width || tileY < 0 || tileY >= tiles_.height)
	{
		return;
	}

	if (mpResource_)
	{
		mpResource_->SetWaveSource(tileX, tileY, radius, intensity, 0.5f);
	}
}

void TileMap::TriggerWaveAtWorldPos(const Vector2& worldPos, float radius, float intensity)
{
	int tileX = static_cast<int>(std::floor(worldPos.x / size_.x));
	int tileY = static_cast<int>(std::floor(worldPos.y / size_.y));
	
	TriggerWaveAtTile(tileX, tileY, radius, intensity);
}

void TileMap::Draw(Render* render, float deltaTime)
{
	if (size_.x <= 0 || size_.y <= 0)
	{
		return;
	}

	auto commdandList = render->GetCommandList();

	mpResource_->DispatchTileDiffusion(commdandList, deltaTime);

	render->Draw(mpResource_.get());
}