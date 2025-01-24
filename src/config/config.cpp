#include "config.h"

// State.
unsigned int Config::currentScore = 0;

// Window.
unsigned int Config::windowWidth = 1440;
unsigned int Config::windowHeight = 1080;

// Camera.
float Config::fieldOfVision = 90.0f;
float Config::lookAtHeight = 0.0f;

// PBR-Parameters.
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
bool Config::showHitbox = false;

// Animations.
float Config::animationSpeed = 0.0f;
float Config::animationLooper = 0.0f;
float Config::debugRotation = 0.0f;
// Realistic rotation would be following: skyRotation = 1.0f / 240000.0f;
float Config::skyRotation = 1.0f / 2400.0f;

// Bill the Salmon.
const float Config::velocityBound = -9.8f / 1000.0f;
const float Config::verticalAcceleration = -0.001f;
const float Config::verticalVelocity = 0.02f;
