#ifndef CONFIG_H
#define CONFIG_H
#include "src/drawables/skybox.h"

/**
 * The global configuration, containing global configuration
 * parameters that are used all over the program.
 */
class Config {
   public:
    static unsigned int currentScore; /**< Current score of the player. */

    static unsigned int windowWidth;  /**< Current width of the window. */
    static unsigned int windowHeight; /**< Current height of the window. */
    static float fieldOfVision;       /**< Field of vision. */

    static float indexOfRefraction; /**< The eta value for cook torrance. */
    static float roughness;         /**< The roughness for cook torrance. */

    // Debug values.
    static float debugRotation;         /**< Amount of debug rotation to apply (used in debug mode). */
    static unsigned int obstacleAmount; /**< Number of obstacles to spawn. */
    static float obstacleLowerBound;    /**< Lower bound of the height of the obstacle. */
    static float obstacleUpperBound;    /**< Upper bound of the height of the obstacle. */
    static float obstacleGapHeight;     /**< Height of the gap of the obstacle. */
    static float obstacleDistance;      /**< Distance between obstacle. */
    static float obstacleWidth;         /**< Width of the obstacle. */
    static float obstacleDepth;         /**< Depth of the obstacle. */
    static float obstacleSpeed;         /**< Distance scrolled per frame. */
    static float animationLooper;       /**< A float that loops through [0, 1] for animation purposes. */
    static float animationSpeed;        /**< The number of steps per ms. */
    static bool showHitbox;             /**< Whether to show the collision-hit-boxes. */

    static float fishFallingAcceleration; /**< Current downward acceleration of fish. */
    static float fishFallingVelocity;     /**< Current vertical velocity of fish. */
    static float gravity;                 /**< Gravity constant for the fish. */
    static float flop;                    /**< The upward acceleration of the flop. */
    static float skyRotation;             /**< The speed at which the skybox rotates. */
    static float lookAtHeight;            /**< Height of the camera. */
};

#endif  // CONFIG_H
