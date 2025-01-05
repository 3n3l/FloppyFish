#ifndef CONFIG_H
#define CONFIG_H

/**
 * The global configuration, containing global configuration
 * parameters that are used all over the program.
 */
class Config {
   public:
    static unsigned int windowWidth;  /**< current width of the window */
    static unsigned int windowHeight; /**< current height of the window */
    static unsigned int currentScore; /**< current score of the player */

    // Debug values.
    static float etaValue;            /**< the eta value for cook torrance */
    static float roughness;           /**< the roughness for cook torrance */
};

#endif  // CONFIG_H
