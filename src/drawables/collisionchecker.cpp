#include "collisionchecker.h"
#include <src/gui/mainwindow.h>

bool CollisionChecker::checkCollision(const std::shared_ptr<FishController>& fish,
                                      const std::shared_ptr<Obstacle>& obstacle) {
    // Get obstacle bounding box (UpperPart and LowerPart)
    float obsX, obsY, obsWidth, obsHeight;
    obstacle->getBounds(obsX, obsY, obsWidth, obsHeight);

    // Check if the rectangles overlap (AABB - Axis-Aligned Bounding Box Collision)
    if ((fish->x() + fish->width() > obsX && fish->x() < obsX + obsWidth && fish->y() + fish->height() > obsY && fish->y() < obsY + obsHeight) || fish->y() <= -0.9f) {
        return true;  // Collision detected
    }

    return false;  // No collision
}
