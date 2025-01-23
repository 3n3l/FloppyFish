#include <memory>
#include <vector>
#define GL_SILENCE_DEPRECATION

#include <QFile>
#include <QOpenGLShaderProgram>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "src/config/config.h"
#include "src/drawables/obstacles/obstacle.h"

Obstacle::Obstacle(float offset, const std::shared_ptr<FloppyMesh>& upperPartMesh,
                   const std::shared_ptr<FloppyMesh>& lowerPartMesh)
    : _upperPart(Part(upperPartMesh)),
      _lowerPart(Part(lowerPartMesh)),
      _offset(offset),
      _height(Config::obstacleGapHeight),
      _width(Config::obstacleWidth),
      _depth(Config::obstacleDepth),
      _lightPosition(glm::vec3(0.0f)),
      _x(0) {}
Obstacle::Obstacle(Obstacle const& o)
    : _upperPart(o._upperPart),
      _lowerPart(o._lowerPart),
      _height(o._height),
      _width(o._width),
      _depth(o._depth),
      _offset(o._offset),
      _lightPosition(o._lightPosition),
      _x(o._x) {}
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

    // Set the light position depending on the height of the lower part.
    _lightPosition = glm::vec3(_xCoordinate, _lowerPart.height(), Config::obstacleDepth / 2);

    // Reset the properties of the upper part of this obstacle.
    _upperPart.setHeight(2 - (_lowerPart.height() + Config::obstacleGapHeight));
    _upperPart.setY(2 - (0.5 * _upperPart.height()));
}

void Obstacle::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    if (isOutOfBounds()) {
        // Place the obstacle to the right of the other obstacles.
        _x += static_cast<float>(Config::obstacleAmount) * Config::obstacleDistance;

        // Reset the individual parts.
        reset();
    }

    // Scroll this obstacle.
    _x += Config::obstacleSpeed;

    _modelViewMatrix = translate(modelViewMatrix, glm::vec3(_x, 0, 0));

    // Scale to width and depth, height is handled by the individual parts.
    _modelViewMatrix = scale(_modelViewMatrix, glm::vec3(_width, 1, _depth));

    // Update the individual parts.
    _upperPart.update(elapsedTimeMs, _modelViewMatrix);
    _lowerPart.update(elapsedTimeMs, _modelViewMatrix);

    // Update the light position.
    _lightPosition.x = _x;
}

void Obstacle::draw(glm::mat4 projectionMatrix, std::vector<glm::vec3> lightPositions) {
    // Draw the individual parts.
    _upperPart.draw(projectionMatrix, lightPositions);
    _lowerPart.draw(projectionMatrix, lightPositions);
}
