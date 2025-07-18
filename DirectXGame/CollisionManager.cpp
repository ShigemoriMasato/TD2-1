#include "CollisionManager.h"

void CollisionManager::AddObject(Object* object) {
	objects.push_back(object);
}

void CollisionManager::CheckCollisions() {

	std::list<Object*>::iterator itrA = objects.begin();
	for (; itrA != objects.end(); ++itrA) {
		std::list<Object*>::iterator itrB = itrA;
		++itrB;
		for (; itrB != objects.end(); ++itrB) {

			if ((*itrA)->tag == (*itrB)->tag) {
				continue;
			}

			if (CollisionChecker(*itrA, *itrB)) {
				(*itrA)->OnCollision(*itrB);
				(*itrB)->OnCollision(*itrA);
			}
		}
	}

	objects.clear();
}
