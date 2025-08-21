#pragma once
#include "Object/Collision.h"
#include <list>

class CollisionManager {
public:
	CollisionManager() = default;
	~CollisionManager() = default;

	void AddObject(Collision* object);

	void CheckCollisions();

private:

	std::list<Collision*> objects;

};
