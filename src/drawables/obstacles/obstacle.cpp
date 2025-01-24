#define GL_SILENCE_DEPRECATION

#include "src/drawables/obstacles/obstacle.h"

#include <QFile>
#include <QOpenGLShaderProgram>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "src/config/config.h"
#include "src/drawables/drawable.h"

Obstacle::Obstacle(float offset, const std::shared_ptr<FloppyMesh>& upperPartMesh,
                   const std::shared_ptr<FloppyMesh>& lowerPartMesh)
    : _upperPart(Part(upperPartMesh)),
      _lowerPart(Part(lowerPartMesh)),
      _offset(offset),
      _height(Config::obstacleGapHeight),
      _width(Config::obstacleWidth),
      _depth(Config::obstacleDepth),
      _xCoordinate(0) {}
Obstacle::Obstacle(Obstacle const& o)
    : _upperPart(o._upperPart),
      _lowerPart(o._lowerPart),
      _height(o._height),
      _width(o._width),
      _depth(o._depth),
      _offset(o._offset),
      _xCoordinate(o._xCoordinate) {}
Obstacle::~Obstacle() {}

void Obstacle::init() {
    // Initialize the individual parts.
    _upperPart.init();
    _lowerPart.init();

    // Place the obstacle to the right of the window.
    _xCoordinate = 1 + _offset + (_width / 2);

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
        _xCoordinate += static_cast<float>(Config::obstacleAmount) * Config::obstacleDistance;

        // Reset the individual parts.
        reset();
    }

    // Scroll this obstacle.
    _xCoordinate += Config::obstacleSpeed;

    _modelViewMatrix = translate(modelViewMatrix, glm::vec3(_xCoordinate, 0, 0));

    // Scale to width and depth, height is handled by the individual parts.
    _modelViewMatrix = scale(_modelViewMatrix, glm::vec3(_width, 1, _depth));

    // Update the individual parts.
    _upperPart.update(elapsedTimeMs, _modelViewMatrix);
    _lowerPart.update(elapsedTimeMs, _modelViewMatrix);
}

void Obstacle::draw(glm::mat4 projectionMatrix) {
    // Draw the individual parts.
    _upperPart.draw(projectionMatrix);
    _lowerPart.draw(projectionMatrix);
}

void Obstacle::getBounds(float& bx, float& by, float& bwidth, float& bheight) const {
    // Combine the bounding boxes of the two parts
    bx = _xCoordinate;
    by = 0.0f;  // The lower part starts at y = 0 (with the upper part above it)
    bwidth = _width;

    // Calculate the height as the height of the lower and upper parts together
    bheight = _lowerPart.height() + _upperPart.height();
}
