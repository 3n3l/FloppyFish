#include "config.h"

// Game variables.
unsigned int Config::currentScore = 0;

// Window state.
// NOTE: arbitrary values, feel free to change.
unsigned int Config::windowWidth = 1440;
unsigned int Config::windowHeight = 1080;
float Config::fieldOfVision = 90.0f;

// PBR Cook-Torrance parameters.
float Config::indexOfRefraction = 1.2f;
float Config::roughness = 0.4f;

// Obstacles.
unsigned int Config::obstacleAmount = 5;
float Config::obstacleLowerBound = 0.33f;
float Config::obstacleUpperBound = 0.66f;
float Config::obstacleGapHeight = 0.2f;
float Config::obstacleDistance = 0.66f;
float Config::obstacleWidth = 0.1f;
float Config::obstacleDepth = 1.0f;
float Config::obstacleSpeed = -0.01f;

// Animations.
float Config::animationSpeed = 0.0f;
float Config::animationLooper = 0.0f;

// Bill the Salmon.
float Config::gravity = 0.005f;
float Config::flop = 0.2f;
