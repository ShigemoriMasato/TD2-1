#include "LevelLoader.h"
#include "Hook/Hook.h"
#include <fstream>
#include <typeinfo>

std::unordered_map<LevelIndex, std::string> LevelLoader::levelFileMap_ = {
	{LevelIndex::Level1,"level1.json"},
	{LevelIndex::Level2,"level2.json"}
};

LevelLoader::LevelLoader()
{
	logger_ = std::make_unique<Logger>();
	logger_->RegistLogFile("LevelLoader");
}

void LevelLoader::LoadLevel(const std::string& filePath, TileMap& tileMap)
{
	auto path = std::filesystem::path(filePath);
	std::ifstream file(path);
	if (!file.is_open())
	{
		throw std::runtime_error("LevelLoader::LoadLevel(" + filePath + ") : Failed to open file.");
	}

	file >> levelData;

	filePath_ = filePath;

	worldWidth_ = levelData.value("width", 0);
	worldHeight_ = levelData.value("height", 0);
	tileWidth_ = levelData.value("tilewidth", 0);
    tileHeight_ = levelData.value("tileheight", 0);

	TileInfo tiles;
	tiles.width = worldWidth_;
	tiles.height = worldHeight_;
	tiles.tileWidth = tileWidth_;
    tiles.tileHeight = tileHeight_;

	tiles.type.reserve(worldWidth_ * worldHeight_);

	for (auto& layer : levelData["layers"])
	{
		if (layer["type"] == "tilelayer")
		{
			const auto& data = layer["data"];
			for (auto& tile : data)
			{
				tiles.type.emplace_back(static_cast<TileType>(tile - 1));
			}
		}
	}

	tileMap.LoadMap(std::move(tiles));
}

void LevelLoader::AddEnemy(EnemyManager& enemyManager)
{
	for (auto& layer : levelData["layers"])
	{
		if (layer["type"] == "objectgroup")
		{
			std::string className = layer.value("class", "");
			std::string layerName = layer.value("name", "");
			for (auto& obj : layer["objects"])
			{
				float x = std::ceil(obj["x"].get<float>());
				float y = std::ceil(obj["y"].get<float>());

				float worldX = x / tileWidth_;
				float worldY = (worldHeight_ - 1 - y / tileHeight_) ;

				if (className == "FloatEnemy")
				{
					enemyManager.SpawnEnemy("FloatEnemy", { worldX, worldY,0.0f });
				}
				if (className == "DashEnemy")
				{
					enemyManager.SpawnEnemy("DashEnemy", { worldX, worldY,0.0f });
				}
				if (className == "DivisionEnemy")
				{
					enemyManager.SpawnEnemy("DivisionEnemy", { worldX, worldY,0.0f });
				}
				if (className == "TrackerEnemy")
				{
					enemyManager.SpawnEnemy("TrackerEnemy", { worldX, worldY,0.0f });
				}
			}
		}
	}
}
