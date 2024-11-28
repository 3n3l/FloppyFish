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
    static float zoomSpeed;           /**< the speed of zooming */
    static float mouseSensitivity;    /**< the sensitivity of moving the camera */
};

#endif  // CONFIG_H