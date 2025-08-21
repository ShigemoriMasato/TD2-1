#include "CollisionManager.h"
#include <Math/MyMath.h>

void CollisionManager::AddObject(Collision* object) {
	objects.push_back(object);
}

void CollisionManager::CheckCollisions() {

	std::list<Collision*>::iterator itrA = objects.begin();
	for (; itrA != objects.end(); ++itrA) {
		std::list<Collision*>::iterator itrB = itrA;
		++itrB;
		for (; itrB != objects.end(); ++itrB) {

			if ((*itrA)->tag_ == (*itrB)->tag_) {
				continue;
			}

			Collision* a = *itrA;
			Collision* b = *itrB;

			if (CollisionChecker) {

			}
		}
	}

	objects.clear();
}
