#pragma once
#include "Tile/TileMap.h"
#include <json.hpp>
#include <map>
#include <optional>

using Json = nlohmann::json;
class LevelLoader
{
public:
	
    void LoadLevel(const std::string& filePath,TileMap& tileMap);

private:

	std::string filePath_;
	
	int worldWidth_;
	int worldHeight_;
private:
	
    template<typename T>
    std::optional<T> GetTileProperty(const Json& tile_json, std::string property_name);
};

template<typename T>
inline std::optional<T> LevelLoader::GetTileProperty(const Json& tile_json, std::string property_name)
{
    if (!tile_json.contains("properties")) return std::nullopt;
    const auto& properties = tile_json["properties"];
    for (const auto& property : properties)
    {
        if (property.contains("name") && property["name"] == std::string(property_name))
        {
            if (property.contains("value"))
            {
                return property["value"].get<T>();
            }
        }
    }
    return std::nullopt;
}
