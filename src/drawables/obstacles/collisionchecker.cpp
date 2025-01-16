#include "collisionchecker.h"

bool CollisionChecker::checkCollision(const FloppyFish& fish, const Obstacle& obstacle) {
    // Get fish bounding box
    float fishX, fishY, fishWidth, fishHeight;
    fish.getBounds(fishX, fishY, fishWidth, fishHeight);

    // Get obstacle bounding box (UpperPart und LowerPart)
    float obsX, obsY, obsWidth, obsHeight;
    obstacle.getBounds(obsX, obsY, obsWidth, obsHeight);

    // Check if the rectangles overlap (AABB - Axis-Aligned Bounding Box Kollision)
    if (fishX + fishWidth > obsX && fishX < obsX + obsWidth && fishY + fishHeight > obsY && fishY < obsY + obsHeight) {
        return true;  // Collision detected
    }

    return false;  // No collision
}
