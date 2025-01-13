#ifndef COLLISIONCHECKER_H
#define COLLISIONCHECKER_H

//#include "floppyfish.h"
#include <src/drawables/obstacles/obstacle.h>

class CollisionChecker {
   public:
    // This method checks whether the FloppyFish collides with an obstacle
    static bool checkCollision(const FloppyFish& fish, const Obstacle& obstacle);
};

#endif  // COLLISIONCHECKER_H
