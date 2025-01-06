#ifndef CONFIG_H
#define CONFIG_H

/**
 * The global configuration, containing global configuration
 * parameters that are used all over the program.
 */
class Config {
   public:
    static unsigned int windowWidth;    /**< current width of the window */
    static unsigned int windowHeight;   /**< current height of the window */
    static unsigned int currentScore;   /**< current score of the player */
    static unsigned int obstacleAmount; /**< number of obstacles to spawn */
    static float obstacleWidth;         /**< width of the obstacle */
    static float obstacleHeight;        /**< height of the obstacle */
    static float obstacleDepth;         /**< depth of the obstacle */
    static float obstacleOffset;        /**< distance between obstacle */
    static float obstacleStep;          /**< distance scrolled per frame */
    static float animationSpeed;        /**< the number of steps per ms */
    static float animationLooper;       /**< a float that loops through [0, 1] for animation purposes */
};

#endif  // CONFIG_H
