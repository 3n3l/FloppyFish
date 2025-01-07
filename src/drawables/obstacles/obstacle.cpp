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
      _height(Config::obstacleGap),
      _width(Config::obstacleWidth),
      _depth(Config::obstacleDepth),
      _texture(texture),
      _offset(offset),
      _x(0) {}
Obstacle::~Obstacle() {}

void Obstacle::init() {
    _upperPart.init();
    _lowerPart.init();

    reset(1 + (1 / _width) + _offset);
}

void Obstacle::reset(float nx) {
    // Reset the x-coordinate to be to the right of the window.
    _x = nx;

    // Reset the properties of the the lower part of this obstacle.
    // Scale obstacle to the configured size, reset _modelViewMatrix.
    float lowerHeight = 0.25f + float(std::rand()) / float(RAND_MAX / 0.75f);
    float lowerY = -(1 / lowerHeight);
    _lowerPart.reset(nx, lowerY, lowerHeight);

    // Reset the properties of the the upper part of this obstacle.
    float upperHeight = 2 - lowerHeight + Config::obstacleGap;
    // float upperY = lowerHeight + Config::obstacleGap;
    float upperY = (lowerHeight + Config::obstacleGap) / upperHeight;
    _upperPart.reset(nx, upperY, upperHeight);

    int i = _offset / Config::obstacleOffset;
    qDebug() << i << "lowerHeight, lowerY: " << lowerHeight << " " << lowerY;
    qDebug() << i << "upperHeight, upperY: " << upperHeight << " " << upperY;
    /*qDebug() << (lowerHeight + upperHeight + Config::obstacleGap == 2);*/

    // Scale obstacle to the configured width and depth.
    _modelViewMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(_width, 1, _depth));

    // Translate to the right of the window.
    _modelViewMatrix = glm::translate(_modelViewMatrix, glm::vec3(_x, 0, 0));
}

void Obstacle::update(float elapsedTimeMs, glm::mat4 modelViewMatrix) {
    // Scroll.
    _x += Config::obstacleStep;
    _modelViewMatrix = glm::translate(_modelViewMatrix, glm::vec3(Config::obstacleStep, 0, 0));

    // Update the separate parts.
    _upperPart.update(elapsedTimeMs, _modelViewMatrix);
    _lowerPart.update(elapsedTimeMs, _modelViewMatrix);
}

void Obstacle::draw(glm::mat4 projectionMatrix) const {
    // Draw the separate parts.
    /*_upperPart.draw(projectionMatrix);*/
    _lowerPart.draw(projectionMatrix);
}
