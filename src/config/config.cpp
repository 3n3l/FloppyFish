#include "config.h"

// Game variables.
unsigned int Config::currentScore = 0;

// Window state.
// NOTE: arbitrary values, feel free to change.
unsigned int Config::windowWidth = 1440;
unsigned int Config::windowHeight = 1080;

// Obstacles.
float Config::obstacleWidth = 0.1f;
float Config::obstacleHeight = 0.5f;
float Config::obstacleDepth = 1.0f;
float Config::obstacleStep = -0.1f;
float Config::obstacleOffset = 5.0f;
unsigned int Config::obstacleAmount = 8;

// Animations.
float Config::animationSpeed = 1.5f;
float Config::animationLooper = 0.0f;
