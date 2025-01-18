#include "config.h"

#include <__system_error/errc.h>

// Game variables.
unsigned int Config::currentScore = 0;

// Window state.
// NOTE: arbitrary values, feel free to change.
unsigned int Config::windowWidth = 1440;
unsigned int Config::windowHeight = 1080;
float Config::fieldOfVision = 90.0f;
float Config::lookAtHeight = 0.0f;

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
bool Config::showHitbox = true;

// Animations.
float Config::animationSpeed = 0.0f;
float Config::animationLooper = 0.0f;
float Config::debugRotation = 0.05f;
// Realistic rotation would be following: skyRotation = 1.0f / 240000.0f;
float Config::skyRotation = 1.0f / 2400.0f;

// Bill the Salmon.
float Config::fishFallingAcceleration = 0.0f;
float Config::fishFallingVelocity = 0.0f;
float Config::gravity = -9.8f / 1000.0f;
float Config::flop = 0.02f;
