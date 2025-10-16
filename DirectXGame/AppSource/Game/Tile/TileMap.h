#pragma once
#include "Math/MyMath.h"
#include <Render/Resource/ModelResource.h>
#include <Render/Render.h>
#include <memory>

enum class TileType
{
    Empty = 0,      // 空
    Solid,          // 通常
    Trap,           // 罠 
    //....追加予定
    Slope_0_1,      // "/" 左: 0.0 右: 1.0
    Slope_0_0_5,    // "/" 左: 0.0 右: 0.5
    Slope_1_0,      // "\" 左: 1.0 右: 0.0
    Slope_1_0_5,    // "\" 左: 1.0 右: 0.5
    Slope_0_5_0,    // "/" 左: 0.5 右: 0.0
    Slope_0_5_1,    // "/" 左: 0.5 右: 1.0

};

struct TileInfo
{
    std::vector<TileType>type;
    int width;
    int height;
};

class PhysicsEngine;
class TileMap
{
public:
    TileMap(PhysicsEngine* physicsEngine);
    ~TileMap();

    void LoadMap(TileInfo&& tiles);

    // タイル情報を取得する、マップ範囲外の場合は nullptr を返す
    TileType GetTileInfoAt(int x,int y) const;

    // 位置によってタイルを取得する
    TileType GetTileTypeAt(int x ,int y) const;
    // ワールド座標からタイルを取得する
    TileType GetTileAtWorldPos(const Vector2& worldPos) const;
    // タイルサイズ(一ブロックの)
    const Vector2& Size()const{return size_;}
    // マップサイズ(マップ全体の)
    const Vector2& MapSize()const{return mapSize_;}
    // ワールドサイズ(ワールド空間上のサイズ)
    Vector2 WorldSize()const;
    //　マップチップ配列
    const TileInfo& Tiles()const{return tiles_;}

    bool IsDraw()const{return isDraw_;}
    void SetDraw(bool isDraw){isDraw_ = isDraw;}
    void SetPhysicsEngine(PhysicsEngine* physicsEngine){physicsEngine_ = physicsEngine;}

    void SetModelData(ModelData* modelData,Camera* camera);

    void Draw(Render* render);
private:
    Vector2 size_ = {1.0f,1.0f};
    Vector2 mapSize_;
    
    TileInfo tiles_;
    std::vector<std::unique_ptr<ModelResource>> models_;

    bool isDraw_ = false;

    PhysicsEngine* physicsEngine_ = nullptr;
};

