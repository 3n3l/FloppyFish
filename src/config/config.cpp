#include "config.h"

// Game variables.
unsigned int Config::currentScore = 0;

// Window state.
// NOTE: arbitrary values, feel free to change.
unsigned int Config::windowWidth = 1440;
unsigned int Config::windowHeight = 1080;

// Obstacles.
float Config::obstacleWidth = 0.1f;
float Config::obstacleGap = 0.4;
float Config::obstacleDepth = 1.0f;
float Config::obstacleStep = -0.01f;
float Config::obstacleOffset = 0.8;
unsigned int Config::obstacleAmount = 6;

// Animations.
float Config::animationSpeed = 0.0f;
float Config::animationLooper = 0.0f;
