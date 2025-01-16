#include "collisionchecker.h"

bool CollisionChecker::checkCollision(const FloppyFish& fish, const Obstacle& obstacle) {
    // Get obstacle bounding box (UpperPart und LowerPart)
    float obsX, obsY, obsWidth, obsHeight;
    obstacle.getBounds(obsX, obsY, obsWidth, obsHeight);

    // Check if the rectangles overlap (AABB - Axis-Aligned Bounding Box Kollision)
    if (fish.x() + fish.width() > obsX && fish.x() < obsX + obsWidth && fish.y() + fish.height() > obsY && fish.y() < obsY + obsHeight) {
        return true;  // Collision detected
    }

    return false;  // No collision
}
