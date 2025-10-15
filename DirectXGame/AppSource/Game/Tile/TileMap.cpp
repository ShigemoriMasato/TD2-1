#include "TileMap.h"


const TileInfo* TileMap::GetTileInfoAt(int x, int y) const
{
    if (x < 0 || x >= mapSize_.x || y < 0 || y >= mapSize_.y)
    {
        return nullptr;
    }
    size_t index = static_cast<size_t>(x + y * mapSize_.x);

    if (index < tiles_.size())
    {
        return &tiles_[index];
    }
    return nullptr;
}

TileType TileMap::GetTileTypeAt(int x, int y) const
{
    const TileInfo* info = GetTileInfoAt(x,y);
    return info ? info->type : TileType::Empty;
}

TileType TileMap::GetTileAtWorldPos(const Vector2& worldPos) const
{
    int x = static_cast<int>(std::floor(worldPos.x / size_.x));
    int y = static_cast<int>(std::floor(worldPos.y / size_.y));

    return GetTileTypeAt(x, y);
}

void TileMap::Draw(Render* render)
{
    if (size_.x <= 0 || size_.y <= 0)
    {
        return;
    }

    for (size_t y = 0; y < mapSize_.y; ++y){
        for (size_t x = 0; x < mapSize_.x; ++x){ 
            size_t index = x + y * size_t(mapSize_.x);

            if (index < tiles_.size() && tiles_[index].model && tiles_[index].type != TileType::Empty)
            {
                const auto& tileInfo = tiles_[index];

                tileInfo.model->position_ = Vector3(x * size_.x, y * size_.y, 0.0f);
                
                render->Draw(tileInfo.model.get());
            }

        }
    }
}