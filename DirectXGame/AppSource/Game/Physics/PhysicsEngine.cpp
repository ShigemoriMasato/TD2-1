#include "PhysicsEngine.h"
#include "PhysicsActor.h"
#include "../Collision/Collider.h"
#include "../BaseObject.h"
#include "../Tile/TileMap.h"
#include <set>
#undef max
#undef min

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
	CheckTileTriggers();
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
			auto tileX = TileXIndex(rightTopX, tileSize.x);
			auto tileY = TileYIndex(objPos.y, tileSize.y, worldSize.y);
			auto tileTypeTop = tile->GetTileTypeAt(tileX, tileY);
			auto tileYBottom = TileYIndex(objPos.y - objHalfSize.y + epsilon, tileSize.y, worldSize.y);
			auto tileTypeBottom = tile->GetTileTypeAt(tileX, tileYBottom);

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
			auto tileTypeTop = tile->GetTileTypeAt(tileX, tileY);

			auto tileYBottom = TileYIndex(objPos.y - objHalfSize.y + epsilon, tileSize.y, worldSize.y);
			auto tileTypeBottom = tile->GetTileTypeAt(tileX, tileYBottom);

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
			auto leftBottomY = newPos.y - objHalfSize.y;
			auto tileY = TileYIndex(leftBottomY, tileSize.y, worldSize.y);
			auto tileX = TileXIndex(objPos.x, tileSize.x);

			auto tileTypeLeft = tile->GetTileTypeAt(tileX, tileY);
			auto tileXRight = TileXIndex(objPos.x + objHalfSize.x - epsilon, tileSize.x);
			auto tileTypeRight = tile->GetTileTypeAt(tileXRight, tileY);

			if (tileTypeLeft == TileType::Solid || tileTypeRight == TileType::Solid)
			{
				newPos.y = (tileY + 1) * tileSize.y + objHalfSize.y;
				actor->velocity_.y = 0.0f;
				actor->collidedBottom_ = true;
			}
			else
			{
				auto widthLeft = newPos.x - tileX * tileSize.x - objHalfSize.x;
				auto widthRight = newPos.x + objHalfSize.x - (tileXRight + 1) * tileSize.x;
				auto heightLeft = GetTileHeightAtWidth(widthLeft, tileTypeLeft, tileSize);
				auto heightRight = GetTileHeightAtWidth(widthRight, tileTypeRight, tileSize);
				auto height = std::max(heightLeft, heightRight);

				if (height > 0.0f)
				{
					if (newPos.y < (tileY + 1) * tileSize.y + objHalfSize.y + height)
					{
						newPos.y = (tileY + 1) * tileSize.y + objHalfSize.y + height;
						actor->velocity_.y = 0.0f;
						actor->collidedBottom_ = true;
					}
				}
			}
		}
		else if (dis.y > 0.0f)
		{
			auto topLeftY = newPos.y - objHalfSize.y;
			auto tileY = TileYIndex(topLeftY, tileSize.y, worldSize.y);

			auto tileX = TileXIndex(objPos.x, tileSize.x);
			auto tileTypeLeft = tile->GetTileTypeAt(tileX, tileY);

			auto tileXRight = TileXIndex(objPos.x + objHalfSize.x - epsilon, tileSize.x);
			auto tileTypeRight = tile->GetTileTypeAt(tileXRight, tileY);

			if (tileTypeLeft == TileType::Solid || tileTypeRight == TileType::Solid)
			{
				newPos.y = tileY * tileSize.y - objHalfSize.y;
				actor->velocity_.y = 0.0f;
				actor->collidedTop_ = true;
			}

		}

	}
	//速度処理完了してから位置を更新
	transform->position = Vector3(newPos) - objPos;
	actor->velocity_.x = std::clamp(actor->velocity_.x, -maxSpeed_, maxSpeed_);
}

void PhysicsEngine::ResolveObjectCollisions(BaseObject* moveObject, BaseObject* other)
{
	auto* moveTransform = moveObject->GetTransform();
	auto* moveCollider = moveObject->GetCollider();
	auto* moveActor = moveObject->GetActor();

	auto* otherCollider = other->GetCollider();

	auto movePos = moveCollider->GetWorldAABB().Position();
	auto otherPos = otherCollider->GetWorldAABB().Position();

	auto moveHalfSize = moveCollider->GetWorldAABB().Size() * 0.5f;
	auto otherHalfSize = otherCollider->GetWorldAABB().Size() * 0.5f;

	Vector2 dis;
	dis.x = std::abs(movePos.x - otherPos.x);
	dis.y = std::abs(movePos.y - otherPos.y);

	auto overlap = Vector2(moveHalfSize + otherHalfSize) - dis;
	if (overlap.x < 0.0f && overlap.y < 0.0f)return;

	if (overlap.x < overlap.y)
	{
		if (movePos.y > otherPos.y)
		{
			moveTransform->position.x -= overlap.x;
			if (moveActor->velocity_.x > 0.0f)
			{
				moveActor->velocity_.x = 0.0f;
				moveActor->collidedRight_ = true;
			}
		}
		else
		{
			moveTransform->position.x += overlap.x;
			if (moveActor->velocity_.x < 0.0f)
			{
				moveActor->velocity_.x = 0.0f;
				moveActor->collidedLeft_ = true;
			}
		}
	}
	else
	{
		if (movePos.x > otherPos.x)
		{
			moveTransform->position.y += overlap.y;
			if (moveActor->velocity_.y < 0.0f)
			{
				moveActor->velocity_.y = 0.0f;
				moveActor->collidedBottom_ = true;
			}
		}
		else
		{
			moveTransform->position.y -= overlap.y;
			if (moveActor->velocity_.y > 0.0f)
			{
				moveActor->velocity_.y = 0.0f;
				moveActor->collidedTop_ = true;
			}
		}
	}

}

void PhysicsEngine::ApplyWorldBounds(PhysicsActor* actor, float deltaTime)
{
	if (!actor || !worldBounds_)return;

	auto* obj = actor->owner_;
	auto* collider = obj->GetCollider();
	auto* transform = obj->GetTransform();
	auto objectWorldAABB = collider->GetWorldAABB();
	auto objPos = objectWorldAABB.Position();
	auto objHalfSize = objectWorldAABB.Size() * 0.5f;

	//左方向
	if (objPos.x < worldBounds_->min.x + objHalfSize.x)
	{
		actor->velocity_.x = 0.0f;
		objPos.x = worldBounds_->min.x + objHalfSize.x;
		actor->collidedLeft_ = true;
	}
	//右方向
	if (objPos.x > worldBounds_->max.x - objHalfSize.x)
	{
		actor->velocity_.x = 0.0f;
		objPos.x = worldBounds_->max.x - objHalfSize.x;
		actor->collidedRight_ = true;
	}
	//上方向
	if (objPos.y > worldBounds_->max.y + objHalfSize.y)
	{
		actor->velocity_.y = 0.0f;
		objPos.y = worldBounds_->max.y + objHalfSize.y;
		actor->collidedTop_ = true;
	}
	//新しい位置 - 現在位置
	transform->position += objPos - objectWorldAABB.Position();
}

float PhysicsEngine::GetTileHeightAtWidth(float width, TileType type, const Vector2& tileSize)
{
	auto relX = std::clamp(width / tileSize.x, 0.0f, 1.0f);

	switch (type)
	{
	case TileType::Slope_0_1:
		return relX * tileSize.y;
	case TileType::Slope_0_0_5:
		return relX * tileSize.y * 0.5f;
	case TileType::Slope_0_5_1:
		return (1.0f - relX) * tileSize.y;
	case TileType::Slope_0_5_0:
		return (1.0f - relX) * tileSize.y * 0.5f;
	case TileType::Slope_1_0:
		return (1.0f - relX) * tileSize.y;
	case TileType::Slope_1_0_5:
		return (1.0f - relX) * tileSize.y * 0.5f;
	}

	return 0.0f;
}

void PhysicsEngine::CheckTileTriggers()
{
	for (auto* actor : actors_)
	{
		if (!actor || !actor->isActive_)continue;
		auto* obj = actor->owner_;
		if(!obj)continue;
		auto* collider = obj->GetCollider();
        if (!collider)continue;

		//ワールド座標を取得
		auto worldAABB = collider->GetWorldAABB();

		std::set<TileType>triggersSet;

		for (auto* tile : collisionTiles_)
		{
			if (!tile)continue;
			const auto& tileSize = tile->Size();
			const auto& tileWorldSize = tile->WorldSize();
			constexpr float epsilon = 1e-6f;

			auto startX = static_cast<int>(std::floor(worldAABB.min.x / tileSize.x));
			auto endX = static_cast<int>(std::ceil((worldAABB.max.x - epsilon) / tileSize.x));
			auto startY =static_cast<int>(std::floor(worldAABB.max.y / tileSize.y));
			auto endY = static_cast<int>(std::ceil((worldAABB.min.y + epsilon) / tileSize.y));

			for (int x = startX; x < endX; x++)
			{
				for (int y = startY; y < endY; y++)
                {
					auto tileType = tile->GetTileTypeAt(x, y);

					switch (tileType)
					{
					case TileType::Trap:
						triggersSet.insert(tileType);
						break;
					default:
						break;
					}

                }
			}
		}
		for (const auto& type : triggersSet)
		{
			tileCollisionInfo_.emplace_back(obj, type);
		}
	}
}

int PhysicsEngine::TileXIndex(float x, float size)
{
	return static_cast<int>(std::floor(x / size));
}

int PhysicsEngine::TileYIndex(float y, float size, float worldY)
{
	return static_cast<int>(std::floor((worldY - 1.0f - y) / size));
}
