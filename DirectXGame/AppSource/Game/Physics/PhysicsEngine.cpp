#include "PhysicsEngine.h"
#include "PhysicsActor.h"
#include "../Collision/Collider.h"
#include "../BaseObject.h"
#include "../Tile/TileMap.h"

void PhysicsEngine::RegisterActor(PhysicsActor* actor)
{
	actors_.push_back(actor);
}

void PhysicsEngine::UnregisterActor(PhysicsActor* actor)
{
	auto it = std::remove(actors_.begin(), actors_.end(), actor);
	actors_.erase(it, actors_.end());
}

void PhysicsEngine::RegisterTileMap(TileMap* tileMap)
{
	collisionTiles_.push_back(tileMap);
}

void PhysicsEngine::UnregisterTileMap(TileMap* tileMap)
{
	auto it = std::remove(collisionTiles_.begin(), collisionTiles_.end(), tileMap);
	collisionTiles_.erase(it, collisionTiles_.end());
}

void PhysicsEngine::Update(float deltaTime)
{
	tileCollisionInfo_.clear();
	objectCollisionInfo_.clear();

	for (auto* actor : actors_)
	{
		if (!actor || !actor->isActive_)continue;

		actor->ResetFlags();

		if (actor->useGravity_)
			actor->force_.y += gravity_ * actor->mass_;

		/*-------------- -
		/別の力にも追加可能
		/---------------*/

		actor->velocity_ += actor->force_ / actor->mass_ * deltaTime;
		actor->ClearForce();

		//マップチップ当たり判定
		ResolveTileCollisions(actor, deltaTime);
		//ワールドに制限
		ApplyWorldBounds(actor, deltaTime);
	}
	//対象のオブジェクトと当たり判定
	CheckAllCollisions();
	//当たる結果を処理
	HandleTileCollision();
}

void PhysicsEngine::CheckAllCollisions()
{
	for (auto itA = actors_.begin(); itA != actors_.end(); ++itA)
	{
		auto* a = (*itA);
		if (!a || !a->isActive_)continue;
		auto* a_owner = a->owner_;
		if (!a_owner)continue;
		auto* a_collider = a_owner->GetCollider();
		if (!a_collider || !a_collider->IsActive())continue;

		for (auto itB = itA + 1; itB != actors_.end(); ++itB)
		{
			auto* b = (*itB);
			if (!b || !b->isActive_)continue;
			auto* b_owner = b->owner_;
			if (!b_owner)continue;
			auto* b_collider = b_owner->GetCollider();
			if (!b_collider || !b_collider->IsActive())continue;


			if (Collision::CheckCollision(*a_collider, *b_collider))
			{
				if (a_collider->GetTag() != ColliderTag::Static && b_collider->GetTag() == ColliderTag::Static)
				{
					ResolveObjectCollisions(a_owner, b_owner);
				}
				else if (a_collider->GetTag() == ColliderTag::Static && b_collider->GetTag() != ColliderTag::Static)
				{
					ResolveObjectCollisions(b_owner, a_owner);
				}
				else
				{
					objectCollisionInfo_.emplace_back(a_owner, b_owner);
				}
			}

		}
	}

}

void PhysicsEngine::ResolveTileCollisions(PhysicsActor* actor, float deltaTime)
{
	auto* obj = actor->owner_;
	if (!obj)return;
	auto* transform = obj->GetTransform();
	auto* collider = obj->GetCollider();
	if (!transform || !collider || collider->IsTrigger())return;
	auto worldAABB = collider->GetWorldAABB();
	if ((worldAABB.min.x > worldAABB.max.x) ||
		(worldAABB.min.y > worldAABB.max.y) ||
		(worldAABB.min.z > worldAABB.max.z))
		return;
	auto objPos = worldAABB.Position();
	auto objHalfSize = worldAABB.Size() * 0.5f;
	constexpr float epsilon = 1e-6f;
	//このフレームの移動量
	auto dis = actor->velocity_ * deltaTime;
	auto newPos = Vector2(objPos) + dis;
	//もしコライダー作動していないならそのまま移動
	if (!collider->IsActive())
	{
		transform->position = dis;
		actor->velocity_.x = std::clamp(actor->velocity_.x, -maxSpeed_, maxSpeed_);
        actor->velocity_.y = std::clamp(actor->velocity_.y, -maxSpeed_, maxSpeed_);
		return;
	}

	for (auto* tile : collisionTiles_)
	{
		if (!tile)continue;
		const auto& tileSize = tile->Size();
		const auto& worldSize = tile->MapSize();

		//水平方向から(右)
		if (dis.x > 0.0f)
		{
			auto rightTopX = newPos.x + objHalfSize.x;
			auto tileX = TileXIndex(rightTopX,tileSize.x);
			auto tileY = TileYIndex(objPos.y, tileSize.y, worldSize.y);
			auto tileTypeTop = tile->GetTileTypeAt( tileX, tileY );
			auto tileYBottom = TileYIndex(objPos.y - objHalfSize.y + epsilon, tileSize.y, worldSize.y);
			auto tileTypeBottom = tile->GetTileTypeAt( tileX, tileYBottom );

			if (tileTypeTop == TileType::Solid || tileTypeBottom == TileType::Solid)
			{
				newPos.x = tileX * tileSize.x - objHalfSize.x;
                actor->velocity_.x = 0.0f;
				actor->collidedRight_ = true;
			}
			else
			{
				auto widthRight = newPos.x + objHalfSize.x - tileX * tileSize.x;
				auto heightRight = GetTileHeightAtWidth(widthRight, tileTypeBottom, tileSize);
				if (heightRight > 0.0f)
				{
					if (newPos.y < (tileYBottom + 1) * tileSize.y + objHalfSize.y + heightRight)
					{
                        newPos.y = (tileYBottom + 1) * tileSize.y + objHalfSize.y + heightRight;
						actor->collidedBottom_ = true;
					}
				}
			}
		}
		//左方向
		else if (dis.x < 0.0f)
		{
			auto leftTopX = newPos.x - objHalfSize.x;
			auto tileX = TileXIndex(leftTopX, tileSize.x);
			auto tileY = TileYIndex(objPos.y, tileSize.y, worldSize.y);
			auto tileTypeTop = tile->GetTileTypeAt( tileX, tileY );

			auto tileYBottom = TileYIndex(objPos.y - objHalfSize.y + epsilon, tileSize.y, worldSize.y);
			auto tileTypeBottom = tile->GetTileTypeAt( tileX, tileYBottom );

			if (tileTypeTop == TileType::Solid || tileTypeBottom == TileType::Solid)
			{
				newPos.x = (tileX + 1) * tileSize.x + objHalfSize.x;
                actor->velocity_.x = 0.0f;
                actor->collidedLeft_ = true;
			}
			else
			{
				auto widthLeft = newPos.x - tileX * tileSize.x - objHalfSize.x;
                auto heightLeft = GetTileHeightAtWidth(widthLeft, tileTypeBottom, tileSize);
				if (heightLeft > 0.0f)
				{
					if (newPos.y < (tileYBottom + 1) * tileSize.y + objHalfSize.y + heightLeft)
					{
                        newPos.y = (tileYBottom + 1) * tileSize.y + objHalfSize.y + heightLeft;
                        actor->collidedBottom_ = true;
					}
				}
			}

		}
		
		//下方向
		if (dis.y < 0.0f)
		{
			//TODO:
		}

	}

}

void PhysicsEngine::ResolveObjectCollisions(BaseObject* moveObject, BaseObject* other)
{
}

void PhysicsEngine::ApplyWorldBounds(PhysicsActor* actor, float deltaTime)
{

}

float PhysicsEngine::GetTileHeightAtWidth(float width, TileType type, const Vector2& tileSize)
{
	return 0.0f;
}

void PhysicsEngine::HandleTileCollision()
{
}

int PhysicsEngine::TileXIndex(float x, float size)
{
	return static_cast<int>(std::floor(x / size));
}

int PhysicsEngine::TileYIndex(float y, float size, float worldY)
{
	return static_cast<int>(std::floor((worldY - 1.0f - y) / size));
}
