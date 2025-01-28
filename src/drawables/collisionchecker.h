#ifndef COLLISIONCHECKER_H
#define COLLISIONCHECKER_H

#include <src/drawables/obstacles/obstacle.h>
#include <src/drawables/fishController.h>

class CollisionChecker {
   public:
    // This method checks whether the FloppyFish collides with an obstacle
    static bool checkCollision(const std::shared_ptr<FishController>& salmon, const std::shared_ptr<Obstacle>& obstacle);
};

#endif // COLLISIONCHECKER_H
