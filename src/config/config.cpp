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
