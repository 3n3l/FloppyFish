#include "src/drawables/obstacles/obstacle.h"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "src/config/config.h"
#include "src/drawables/drawable.h"

#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/gltypes.h>

#include <QDebug>
#include <QFile>
#include <QOpenGLShaderProgram>
#include <QTextStream>
#include <string>

#include "glm/fwd.hpp"

Obstacle::Obstacle(std::string texture, float offset)
    : Drawable(),
      _upperPart(Part("/res/sign.png")),
      _lowerPart(Part("/res/lamp.png")),
      _height(Config::obstacleGapHeight),
      _width(Config::obstacleWidth),
      _depth(Config::obstacleDepth),
      _texture(texture),
      _offset(offset),
      _x(0) {}
Obstacle::~Obstacle() {}

void Obstacle::init() {
    // Initialize the individual parts.
    _upperPart.init();
    _lowerPart.init();

    // Place the obstacle to the right of the window.
    _x = 1 + _offset + (_width / 2);

    // Reset the individual parts.
    reset();
}

void Obstacle::reset() {
    // Reset the properties of the lower part of this obstacle.
    float lower = Config::obstacleLowerBound;
    float upper = Config::obstacleUpperBound;
    _lowerPart.setHeight(lower + float(std::rand()) / float(RAND_MAX / (upper - lower)));
    _lowerPart.setY((0.5 * _lowerPart.height()) - 1);

    // Reset the properties of the upper part of this obstacle.
    _upperPart.setHeight(2 - (_lowerPart.height() + Config::obstacleGapHeight));
    _upperPart.setY(2 - (0.5 * _upperPart.height()));
}

void Obstacle::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    if (isOutOfBounds()) {
        // Place the obstacle to the right of the other obstacles.
        _x += (Config::obstacleAmount * Config::obstacleDistance);

        // Reset the individual parts.
        reset();
    }

    // Scroll this obstacle.
    _x += Config::obstacleSpeed;

    _modelViewMatrix = glm::translate(modelViewMatrix, glm::vec3(_x, 0, 0));

    // Scale to width and depth, height is handled by the individual parts.
    _modelViewMatrix = glm::scale(_modelViewMatrix, glm::vec3(_width, 1, _depth));

    // Update the individual parts.
    _upperPart.update(elapsedTimeMs, _modelViewMatrix);
    _lowerPart.update(elapsedTimeMs, _modelViewMatrix);
}

void Obstacle::draw(glm::mat4 projectionMatrix) const {
    // Draw the individual parts.
    _upperPart.draw(projectionMatrix);
    _lowerPart.draw(projectionMatrix);
}
