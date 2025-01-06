#include "config.h"

// Game variables.
unsigned int Config::currentScore = 0;

// Window state.
// NOTE: arbitrary values, feel free to change.
unsigned int Config::windowWidth = 1440;
unsigned int Config::windowHeight = 1080;

// Obstacles.
unsigned int Config::obstacleAmount = 3;
float Config::obstacleWidth = 0.1;
float Config::obstacleHeight = 0.5;
float Config::obstacleDepth = 1.0;
float Config::obstacleStep = -0.1;
float Config::obstacleOffset = 5.0;

// Animations.
float Config::animationSpeed = 1.5f;
float Config::animationLooper = 0.0f;
