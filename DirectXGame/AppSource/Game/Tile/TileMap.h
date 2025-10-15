#pragma once
#include "Math/MyMath.h"
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>
#include <memory>

enum class TileType
{
    Empty = 0,  // 空
    Solid,     // 通常
    //....追加予定
};

struct TileInfo
{
    std::unique_ptr<ModelResource> model;
    TileType type;
};

class PhysicsEngine;
class TileMap
{
public:
    TileMap(const PhysicsEngine* physicsEngine,const Vector2& size,const Vector2& mapSize,std::vector<TileInfo>&& tiles)
        : physicsEngine_(physicsEngine), size_(size), mapSize_(mapSize),tiles_(std::move(tiles))
    {
        tiles_.resize(static_cast<size_t>(size_.x * size_.y));
    }

    // タイル情報を取得する、マップ範囲外の場合は nullptr を返す
    const TileInfo* GetTileInfoAt(int x,int y) const;

    // 位置によってタイルを取得する
    TileType GetTileTypeAt(int x ,int y) const;
    // ワールド座標からタイルを取得する
    TileType GetTileAtWorldPos(const Vector2& worldPos) const;
    // タイルサイズ(一ブロックの)
    const Vector2& Size()const{return size_;}
    // マップサイズ(マップ全体の)
    const Vector2& MapSize()const{return mapSize_;}
    // ワールドサイズ(ワールド空間上のサイズ)
    const Vector2& WorldSize()const{return size_ * mapSize_;}
    //　マップチップ配列
    const std::vector<TileInfo>& Tiles()const{return tiles_;}

    bool IsDraw()const{return isDraw_;}
    void SetDraw(bool isDraw){isDraw_ = isDraw;}
    void SetPhysicsEngine(const PhysicsEngine* physicsEngine){physicsEngine_ = physicsEngine;}

    void Draw(Render* render);
private:
    Vector2 size_;
    Vector2 mapSize_;
    std::vector<TileInfo> tiles_;

    bool isDraw_ = false;

    const PhysicsEngine* physicsEngine_ = nullptr;
};

