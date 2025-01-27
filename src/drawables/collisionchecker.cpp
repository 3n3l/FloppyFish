#include "collisionchecker.h"
#include <src/gui/mainwindow.h>

bool CollisionChecker::checkCollision(const std::shared_ptr<FishController>& fish,
                                      const std::shared_ptr<Obstacle>& obstacle) {
    // Get obstacle bounding box (UpperPart and LowerPart)
    float obsX, obsY, obsWidth, obsHeight;
    obstacle->getBounds(obsX, obsY, obsWidth, obsHeight);

    // Check if the rectangles overlap (AABB - Axis-Aligned Bounding Box Kollision)
    if ((fish->position().x + fish->width() > obsX && fish->position().x < obsX + obsWidth &&
         fish->position().y + fish->height() > obsY && fish->position().y < obsY + obsHeight) ||
        fish->position().y <= -0.9f) {
        return true;  // Collision detected
    }

    return false;  // No collision
}
