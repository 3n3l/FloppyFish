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
    static float obstacleLowerBound;    /**< lower bound of the height of the obstacle */
    static float obstacleUpperBound;    /**< upper bound of the height of the obstacle */
    static float obstacleGapHeight;     /**< height of the gap of the obstacle */
    static float obstacleDistance;      /**< distance between obstacle */
    static float obstacleWidth;         /**< width of the obstacle */
    static float obstacleDepth;         /**< depth of the obstacle */
    static float obstacleSpeed;         /**< distance scrolled per frame */
    static float animationLooper;       /**< a float that loops through [0, 1] for animation purposes */
    static float animationSpeed;        /**< the number of steps per ms */
};

#endif  // CONFIG_H
