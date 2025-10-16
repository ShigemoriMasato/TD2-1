#pragma once
#include "Math/MyMath.h"
#include "../Tile/TileMap.h"
#include <vector>
#include <optional>

class BaseObject;
class PhysicsActor;
class PhysicsEngine
{
public:

	using ObjectCollision = std::pair<BaseObject*, BaseObject*>;
	using TileCollision = std::pair<BaseObject*, TileType>;

	// アクターを登録
	void RegisterActor(PhysicsActor* actor);
	// アクターを削除
	void UnregisterActor(PhysicsActor* actor);
	// タイルマップを登録
	void RegisterTileMap(TileMap* tileMap);
	// タイルマップを削除
	void UnregisterTileMap(TileMap* tileMap);

	void Update(float deltaTime);

	float Gravity() const { return gravity_; }
	void SetMaxSpeed(float speed) { maxSpeed_ = speed; }
	float MaxSpeed() const { return maxSpeed_; }
	void SetWorldBounds(const AABB& bounds) { worldBounds_ = bounds; }
	const std::optional<AABB>& GetWorldBounds() const { return worldBounds_; }

	
	//現在のフレイムで発生したオブジェクト間の判定情報を取得
	const std::vector<ObjectCollision>& GetCollisionInfo()const { return objectCollisionInfo_; }
    //現在のフレイムで発生したタイルマップの判定情報を取得
	const std::vector<TileCollision>& GetTileCollisionInfo()const { return tileCollisionInfo_; }

private:

	std::vector<PhysicsActor*> actors_;			// 物理演算を行うアクター
	std::vector<TileMap*> collisionTiles_;		// コリジョン判定を行うタイル
	std::optional<AABB> worldBounds_;			// ワールドの範囲

	std::vector<ObjectCollision> objectCollisionInfo_; // 記録された判定を行うオブジェクト
	std::vector<TileCollision> tileCollisionInfo_;		// 記録された判定を行うタイル

	float maxSpeed_ = 20.0f;

	static const inline float gravity_ = 9.8f;

private:
	//全ての判定を行う
	void CheckAllCollisions();
	//マップチップ当たり判定を記録
	void ResolveTileCollisions(PhysicsActor* actor, float deltaTime);
	//オブジェクト間の当たり判定
	void ResolveObjectCollisions(BaseObject* moveObject, BaseObject* other);
	//ワールドの範囲内に収まるようにする
	void ApplyWorldBounds(PhysicsActor* actor, float deltaTime);
	//タイルマップの高さを取得する(斜め移動)
	float GetTileHeightAtWidth(float width, TileType type, const Vector2& tileSize);
	//タイルマップの当たり判定押し戻し
	void CheckTileTriggers();

	int TileXIndex(float x, float size);
	int TileYIndex(float y, float size,float worldY);
};
