#include "CollisionManager.h"

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

			if ((*itrA)->tag_ == "Enemy" && (*itrB)->tag_.find("Buff") != std::string::npos ||
				(*itrB)->tag_ == "Enemy" && (*itrA)->tag_.find("Buff") != std::string::npos) {
				continue;
			}

			if (CollisionChecker(*itrA, *itrB)) {
				if ((*itrA)->onCollision_) {
					(*itrA)->onCollision_(*itrB);
				}

				if ((*itrB)->onCollision_) {
					(*itrB)->onCollision_(*itrA);
				}
			}
		}
	}

	objects.clear();
}
