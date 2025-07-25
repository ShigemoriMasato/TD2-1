#pragma once
#include "Object/Object.h"
#include <list>

class CollisionManager {
public:
	CollisionManager() = default;
	~CollisionManager() = default;

	void AddObject(Object* object);

	void CheckCollisions();

private:

	std::list<Object*> objects;

};
