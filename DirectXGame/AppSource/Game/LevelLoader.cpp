#include "LevelLoader.h"
#include <fstream>

void LevelLoader::LoadLevel(const std::string& filePath, TileMap& tileMap)
{
	auto path = std::filesystem::path(filePath);
	std::ifstream file(path);
	if (!file.is_open())
	{
		throw std::runtime_error("LevelLoader::LoadLevel(" + filePath + ") : Failed to open file.");
	}

	Json jsonData;

	file >> jsonData;

	filePath_ = filePath;

	worldWidth_ = jsonData.value("width", 0);
	worldHeight_ = jsonData.value("height", 0);

	TileInfo tiles;
	tiles.width = worldWidth_;
	tiles.height = worldHeight_;
	tiles.type.reserve(worldWidth_ * worldHeight_);

	for (auto& layer : jsonData["layers"])
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
