#include "src/drawables/obstacle.h"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "src/config/config.h"
#include "src/drawables/drawable.h"
#include "src/utils/utils.h"

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
#include "src/utils/utils.h"

Obstacle::Obstacle(std::string texture, float offset)
    : Drawable(),
      _height(Config::obstacleHeight),
      _width(Config::obstacleWidth),
      _depth(Config::obstacleDepth),
      _texture(texture),
      _offset(offset),
      _x(0) {}
Obstacle::~Obstacle() {}

void Obstacle::init() {
    // Create a program for this class.
    _program = glCreateProgram();

    // Create texture handle.
    _textureHandle = Utils::loadTexture(_texture);

    // Compile shader.
    GLuint vs = Utils::compileShader(GL_VERTEX_SHADER, "src/shaders/obstacle.vs.glsl");
    GLuint fs = Utils::compileShader(GL_FRAGMENT_SHADER, "src/shaders/obstacle.fs.glsl");

    // Attach shader to the program.
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);

    // Link program.
    _program = Utils::linkProgram(_program);

    // Set up a vertex array object for the geometry.
    if (_vertexArrayObject == 0) {
        glGenVertexArrays(1, &_vertexArrayObject);
    }
    glBindVertexArray(_vertexArrayObject);

    // Fill position buffer with data.
    std::vector<glm::vec3> positions = {
        glm::vec3(-1, -1, 0),
        glm::vec3(-1, 1, 0),
        glm::vec3(1, 1, 0),
        glm::vec3(1, -1, 0),
    };
    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Fill the texture coordinates buffer with data.
    std::vector<glm::vec2> texcoords = {
        glm::vec2(-1, -1),
        glm::vec2(-1, 1),
        glm::vec2(1, 1),
        glm::vec2(1, -1),
    };
    GLuint texture_coordinate_buffer;
    glGenBuffers(1, &texture_coordinate_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, texture_coordinate_buffer);
    // Size of two, due to coordinates s and t.
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * 2 * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    // Use index '2', and size of two again.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    // Use index '2', and size of two again.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Unbind vertex array object.
    glBindVertexArray(0);
    // Delete buffers (the data is stored in the vertex array object).
    glDeleteBuffers(1, &position_buffer);
    glDeleteBuffers(1, &texture_coordinate_buffer);

    // Unbind vertex array object.
    glBindVertexArray(0);

    // Set size and position.
    reset(1 + (1 / _width) + _offset);
}

void Obstacle::reset(float newX) {
    // Reset the x-coordinate to be to the right of the window.
    _x = newX;

    // Scale obstacle to the configured size, reset _modelViewMatrix.
    float height = 0.25 + float(std::rand()) / float(RAND_MAX / 0.75);
    _modelViewMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(_width, height, _depth));

    // Translate to the right of the window.
    _modelViewMatrix = glm::translate(_modelViewMatrix, glm::vec3(_x, -(1 / height), 0));
}

void Obstacle::update(float elapsedTimeMs) {
    // Scroll.
    _x += Config::obstacleStep;
    _modelViewMatrix = glm::translate(_modelViewMatrix, glm::vec3(Config::obstacleStep, 0, 0));
}

void Obstacle::draw(glm::mat4 projectionMatrix) const {
    if (_program == 0) {
        qDebug() << "Program not initialized.";
        return;
    }

    // Load program.
    glUseProgram(_program);
    glCheckError();

    // Bind vertex array object.
    glBindVertexArray(_vertexArrayObject);
    glCheckError();

    // Set parameter.
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection_matrix"), 1, GL_FALSE,
                       glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(_program, "modelview_matrix"), 1, GL_FALSE,
                       glm::value_ptr(_modelViewMatrix));

    // Set the background texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureHandle);
    glUniform1i(glGetUniformLocation(_program, "backgroundTexture"), 0);

    // Repeat the background texture horizontally.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // Stretch the background texture vertically.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Call draw.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    glCheckError();

    // Unbind vertex array object.
    glBindVertexArray(0);
    glCheckError();
}
