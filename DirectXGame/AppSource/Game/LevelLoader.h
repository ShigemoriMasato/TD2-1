#pragma once
#include "Tile/TileMap.h"
#include "Enemy/EnemyManager.h"
#include <json.hpp>
#include <map>
#include <optional>

enum class LevelIndex {
    Level0,
    Level1,
    Level2,

	kNumLevels
};

using Json = nlohmann::json;
class LevelLoader
{
public:
	LevelLoader();

    void LoadLevel(const std::string& filePath,TileMap& tileMap);

    template<class T>
    void AddGameObject(std::vector<std::unique_ptr<BaseObject>>& gameObjects,
        ModelManager* modelManager,
        int modelHandle,
        Camera* camera,
        PhysicsEngine* physicsEngine = nullptr,
        bool useGravity = true);

    void AddEnemy(EnemyManager& enemyManager);

    static std::unordered_map<LevelIndex, std::string> GetLevelFileMap() { return levelFileMap_; };

private:
    Json levelData;
	std::string filePath_;
	
	int worldWidth_;
	int worldHeight_;
    int tileWidth_;
    int tileHeight_;

    static std::unordered_map<LevelIndex, std::string> levelFileMap_;

private:
    std::unique_ptr<Logger> logger_;

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
template<typename T>
inline void LevelLoader::AddGameObject(
    std::vector<std::unique_ptr<BaseObject>>& gameObjects,
    ModelManager* modelManager,
    int modelHandle,
    Camera* camera,
    PhysicsEngine* physicsEngine,
    bool useGravity)
{
    const std::string targetType = T::TypeName();

    logger_->Log("Adding " + targetType);

    for (const auto& layer : levelData["layers"])
    {
        if (layer["type"] == "objectgroup")
        {
            std::string className = layer.value("class", "");
            for (auto& obj : layer["objects"])
            {
                if (className == targetType)
                {
                    float x = std::ceil(obj["x"].get<float>());
                    float y = std::ceil(obj["y"].get<float>());

                    float worldX = x / tileWidth_;
                    float worldY = (worldHeight_ - 1 - y / tileHeight_);

                    auto gameObject = std::make_unique<T>();
                    gameObject->Initialize(modelManager->GetModelData(modelHandle), camera);
                    gameObject->SetPosition({ worldX, worldY ,0.0f });
                    if(physicsEngine)
                        gameObject->SetActor(physicsEngine, useGravity);

                    gameObjects.push_back(std::move(gameObject));
                }
            }
        }
    }
}
