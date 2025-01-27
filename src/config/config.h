#ifndef CONFIG_H
#define CONFIG_H

/**
 * The global configuration, containing global configuration
 * parameters that are used all over the program.
 */
class Config {
   public:
    static unsigned int currentScore;        /**< Current score of the player. */
    static unsigned int windowWidth;         /**< Current width of the window. */
    static unsigned int windowHeight;        /**< Current height of the window. */
    static float fieldOfVision;              /**< Field of vision. */
    static float indexOfRefraction;          /**< The eta value for cook torrance. */
    static float roughness;                  /**< The roughness for cook torrance. */
    static float debugRotation;              /**< Amount of debug rotation to apply (used in debug mode). */
    static bool showHitbox;                  /**< Whether to show the collision-hit-boxes. */
    static unsigned int obstacleAmount;      /**< Number of obstacles to spawn. */
    static float obstacleInitialOffset;      /**< Initial offset to the right of the window. */
    static float obstacleLeftOverhang;       /**< Overhang to the left of the window. */
    static float obstacleLowerBound;         /**< Lower bound of the height of the obstacle. */
    static float obstacleUpperBound;         /**< Upper bound of the height of the obstacle. */
    static float obstacleGapHeight;          /**< Height of the gap of the obstacle. */
    static float obstacleDistance;           /**< Distance between obstacle. */
    static float obstacleWidth;              /**< Width of the obstacle. */
    static float obstacleDepth;              /**< Depth of the obstacle. */
    static float obstacleSpeed;              /**< Distance scrolled per frame. */
    static float animationLooper;            /**< A float that loops through [0, 1] for animation purposes. */
    static float animationSpeed;             /**< The number of steps per ms. */
    static const float verticalAcceleration; /**< Downward acceleration of the fish, will be added to the velocity. */
    static const float velocityBound;        /**< Lower bound for the downward velocity of the fish. */
    static const float verticalVelocity;     /**< Upward velocity for the flop motion of the fish. */
    static float skyRotation;                /**< Speed at which the skybox rotates. */
    static float lookAtHeight;               /**< Height of the camera. */
    static const float lowerAngle;           /**< Lower angle for the flopping animation. */
    static const float upperAngle;           /**< Upper angle for the flopping animation. */
    static unsigned int resolutionScale;     /**< Resolution scale factor, used by high DPI screens */
    static float gamma;                      /**< Gamma correction coefficient */
};

#endif  // CONFIG_H
